#pragma once

#include <cuda_runtime_api.h>
#include <glib.h>
#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "gstnvdsmeta.h"
#include "nvds_analytics_meta.h"
#include "nvds_yml_parser.h"
#include "nvdsmeta_schema.h"
#include "yaml-cpp/yaml.h"

namespace optriment {

class Pipeline {
   public:
    Pipeline(const std::string& pipeline_name);
    ~Pipeline();

    Pipeline& addElement(const std::string& factory_name, const std::string& element_name);
    Pipeline& addElement(const std::string& factory_name, const std::string& element_name,
                         const std::string& property_name, const std::string& property_value);

    void run();
    void stop();
    void checkElementsOk() const;

    GstElement* getElement(const std::string& name);
    GstElement* getPipeline() const;
    GMainLoop* getLoop() const;

    void linkElements(std::initializer_list<std::string> elementNames);
    void linkWithRequestPad(const std::string& sinkElementName, const std::string& srcElementName);
    void linkWithRequestPadTee(const std::string& sinkElementName, const std::string& srcElementName);

    typedef GstPadProbeReturn (*ProbeCallback)(GstPad*, GstPadProbeInfo*, gpointer);
    void attachProbe(ProbeCallback cb, const std::string& elementName, const std::string& padName);

   private:
    GstElement* pipeline;
    GMainLoop* loop = nullptr;
    std::string input_file_path;
    std::unordered_map<std::string, GstElement*> stages;
    guint bus_watch_id = 0;
    GstBus* bus = NULL;

    void internalLinkWithRequestPad(const std::string& sinkElementName, const std::string& srcElementName,
                                    const std::string& sinkName, const std::string& srcName);
    void configErrorBus();
    void log(const std::string& message) const;
    void log(const char* message) const;
};
}  // namespace optriment