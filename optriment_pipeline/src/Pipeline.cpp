#include "Pipeline.h"

using namespace optriment;

static gboolean
bus_call (GstBus * bus, GstMessage * msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;
  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
      g_print("End of stream\n");
      g_main_loop_quit(loop);
      break;
    case GST_MESSAGE_ERROR:{
      gchar *debug = NULL;
      GError *error = NULL;
      gst_message_parse_error (msg, &error, &debug);
      g_print("ERROR from element %s: %s\n",
          GST_OBJECT_NAME (msg->src), error->message);
      if (debug)
        g_print("Error details: %s\n", debug);
      g_free(debug);
      g_error_free(error);
      g_main_loop_quit(loop);
      break;
    }
    default:
      break;
  }
  return TRUE;
}

Pipeline::Pipeline(const std::string& pipeline_name)
{
    // Create an empty pipeline
    pipeline = gst_pipeline_new(pipeline_name.c_str());
    if (!pipeline) {
        std::cerr << "Failed to create pipeline" << std::endl;
        exit(-1);
    }
    loop = g_main_loop_new(NULL, FALSE);
}

Pipeline::~Pipeline() {
    // Clean up and release all resources
    if (pipeline) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(pipeline));
    }
}

void Pipeline::configErrorBus()
{
    bus = gst_pipeline_get_bus(GST_PIPELINE (getPipeline()));
    bus_watch_id = gst_bus_add_watch(bus, bus_call, getLoop());
    gst_object_unref(bus);
}

Pipeline& Pipeline::addElement(const std::string& factory_name, const std::string& element_name)
{
    return addElement(factory_name, element_name, "", "");
}

Pipeline& Pipeline::addElement(const std::string& factory_name, const std::string& element_name,
const std::string& property_name, const std::string& property_value)
{
    GstElement* element = gst_element_factory_make(factory_name.c_str(), element_name.c_str());
    if (!element) {
        std::cerr << "Failed to create element: " << element_name << std::endl;
        exit(-1);
    }
    stages.insert({element_name, element});
    gst_bin_add(GST_BIN(pipeline), element);
    if (property_name != "" && property_value != "") {
        g_object_set(G_OBJECT(element), property_name.c_str(), property_value.c_str(), NULL);
    }
    return *this;
}

// Getter for pipeline
GstElement* Pipeline::getPipeline() const {
    return pipeline;
}

void Pipeline::checkElementsOk() const 
{
    for (const auto& pair : stages)
    {
        if (pair.second == nullptr)
        {
            std::cerr << "Element " << pair.first << " could not be created. Exiting." << std::endl;
        }
    }
}

void Pipeline::run()
{
    configErrorBus();

    g_print("Running... \n");
    g_main_loop_run(loop);
    g_print("Returned, stopping playback\n");

    stop();
}

void Pipeline::stop()
{
    gst_element_set_state(getPipeline(), GST_STATE_NULL);
    g_print("Deleting pipeline\n");
    gst_object_unref(GST_OBJECT (getPipeline()));
    g_source_remove(bus_watch_id);
    g_main_loop_unref(loop);
}

GstElement* Pipeline::getElement(const std::string& name)
{
    return stages[name];
}

GMainLoop* Pipeline::getLoop() const
{
    return loop;
}

void Pipeline::linkElements(std::initializer_list<std::string> elementNames) {
    std::vector<GstElement*> elements;

    for (const std::string& name : elementNames) {
        elements.push_back(getElement(name));
    }

    for (int i = 0; i < elements.size()-1; i++) {
        if (!gst_element_link (elements[i], elements[i + 1]))
        {
            log("Elements could not be linked. Exiting.\n");
            exit(-1);
        }
    }
}

void Pipeline::linkWithRequestPad(const std::string& sinkElementName, const std::string& srcElementName)
{
    return internalLinkWithRequestPad(sinkElementName, srcElementName, "sink_0", "src");
}

void Pipeline::linkWithRequestPadTee(const std::string& sinkElementName, const std::string& srcElementName)
{
    return internalLinkWithRequestPad(srcElementName, sinkElementName, "src_%u", "sink");
}

void Pipeline::internalLinkWithRequestPad(const std::string& sinkElementName, const std::string& srcElementName, const std::string& sinkName, const std::string& srcName)
{
    GstPad *sinkpad, *srcpad;
    sinkpad = gst_element_request_pad_simple(getElement(sinkElementName), sinkName.c_str());
    if (!sinkpad) {
        log(sinkElementName);
        log(" request sink pad failed. Exiting.\n");
        exit(-1);
    }

    srcpad = gst_element_get_static_pad(getElement(srcElementName), srcName.c_str());
    if (!srcpad) {
        log(srcElementName);
        log(" request src pad failed. Exiting.\n");
        exit(-1);
    }

    if (gst_pad_link(srcpad, sinkpad) != GST_PAD_LINK_OK && gst_pad_link(sinkpad, srcpad) != GST_PAD_LINK_OK) {
        log("Failed to link. Exiting.\n");
        exit(-1);
    }

    gst_object_unref(sinkpad);
    gst_object_unref(srcpad);
}

void Pipeline::log(const std::string& message) const
{
    g_printerr(message.c_str());
}

void Pipeline::log(const char* message) const
{
    g_printerr(message);
}