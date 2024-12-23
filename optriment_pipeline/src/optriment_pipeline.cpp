#include <gst/gst.h>
#include <glib.h>

#include "Pipeline.h"
#include "ArgParser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime_api.h>
#include "nvds_yml_parser.h"
#include "nvds_analytics_meta.h"
#include "nvdsmeta_schema.h"

#include "gstnvdsmeta.h"


#define RETURN_ON_PARSER_ERROR(parse_expr) \
  if (NVDS_YAML_PARSER_SUCCESS != parse_expr) { \
    g_printerr("Error in parsing configuration file.\n"); \
    return -1; \
  }

void initializer(int argc, char *argv[])
{
    gst_init (&argc, &argv);
    int current_device = -1;
    cudaGetDevice(&current_device);
    struct cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, current_device);
}

int main (int argc, char *argv[])
{
    optriment::ArgParser arg_parser(argc, argv);
    if (!arg_parser.hasOption("--config") || !arg_parser.hasOption("--roi1") || !arg_parser.hasOption("--roi2")) {
        g_printerr ("Usage: %s --config <config-file-path> --roi1 <roi1-config-file> --roi2 <roi2-config-file>\n", argv[0]);
        return -1;
    }
    initializer(argc, argv);
    std::string roi1_config = arg_parser.getOptionValue("--roi1");
    std::string roi2_config = arg_parser.getOptionValue("--roi2");
    NvDsGieType pgie_type;

    optriment::Pipeline pipeline("optriment-pipeline");
    // Create elements for the pipeline
    pipeline.addElement("filesrc", "file-source")
    .addElement("h264parse", "h264-parser")
    .addElement("nvv4l2decoder", "nvv4l2-decoder")
    .addElement("nvstreammux", "stream-muxer")
    .addElement("nvtracker", "tracker")
    .addElement("nvdsanalytics", "analytics1", "config-file", roi1_config)
    .addElement("nvdsanalytics", "analytics2", "config-file", roi2_config)
    .addElement("nvinfer", "pgie")
    .addElement("nvvideoconvert", "nvvideo-converter")
    .addElement("nvdsosd", "on-screen-display")
    // Tee to stream video and Kafka messages in parallel
    .addElement("tee", "broker-video-tee")
    .addElement("queue", "broker-tee")
    .addElement("nvmsgbroker", "msg-broker")
    .addElement("queue", "video-tee")
    .addElement("nvmsgconv", "msg-converter")
    .addElement("nveglglessink", "video-renderer");

    char* file_path = arg_parser.getOptionValue("--config");
    RETURN_ON_PARSER_ERROR(nvds_parse_gie_type(&pgie_type, file_path,
        "primary-gie"));

    RETURN_ON_PARSER_ERROR(nvds_parse_file_source(pipeline.getElement("file-source"), file_path, "source"));
    RETURN_ON_PARSER_ERROR(nvds_parse_streammux(pipeline.getElement("stream-muxer"), file_path, "streammux"));
    RETURN_ON_PARSER_ERROR(nvds_parse_gie(pipeline.getElement("pgie"), file_path, "primary-gie"));
    RETURN_ON_PARSER_ERROR(nvds_parse_tracker(pipeline.getElement("tracker"), file_path, "tracker"));

    RETURN_ON_PARSER_ERROR(nvds_parse_msgconv (pipeline.getElement("msg-converter"), file_path, "msgconv"));
    RETURN_ON_PARSER_ERROR(nvds_parse_msgbroker(pipeline.getElement("msg-broker"), file_path, "msgbroker"));

    if (!pipeline.getPipeline()) {
        g_printerr ("One element could not be created. Exiting.\n");
        return -1;
    }
    pipeline.checkElementsOk();

    // Linking.
    pipeline.linkWithRequestPad("stream-muxer", "nvv4l2-decoder");
    pipeline.linkElements({"file-source", "h264-parser", "nvv4l2-decoder"});
    pipeline.linkElements({"stream-muxer","pgie", "tracker", "analytics1",
                           "analytics2", "nvvideo-converter",
                           "on-screen-display", "broker-video-tee"});
    pipeline.linkElements({"broker-tee", "msg-converter", "msg-broker"});
    pipeline.linkElements({"video-tee", "video-renderer"});

    pipeline.linkWithRequestPadTee("broker-tee", "broker-video-tee");
    pipeline.linkWithRequestPadTee("video-tee", "broker-video-tee");

    gst_element_set_state(pipeline.getPipeline(), GST_STATE_PLAYING);
    pipeline.run();
    return 0;
}
