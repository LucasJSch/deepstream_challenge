// TODO: Delete unnecessary includes and reorganize according to style guide.
#include <gst/gst.h>
#include <glib.h>

#include "Pipeline.h"
#include "ArgParser.h"
#include "KafkaPublisher.h"
#include "RegionCountMessage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cuda_runtime_api.h>
#include "nvds_yml_parser.h"
#include "nvds_analytics_meta.h"
#include "nvdsmeta_schema.h"

#include "gstnvdsmeta.h"

#include <librdkafka/rdkafkacpp.h>

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

static GstPadProbeReturn probe (GstPad * pad, GstPadProbeInfo * info, gpointer u_data, optriment::KafkaPublisher& publisher)
{
    NvDsObjectMeta *obj_meta = NULL;
        NvDsMetaList * l_frame = NULL;
        NvDsMetaList * l_obj = NULL;
    
        GstBuffer* buffer = gst_pad_probe_info_get_buffer(info);
        NvDsBatchMeta* batch_meta = gst_buffer_get_nvds_batch_meta(buffer);

        std::shared_ptr<optriment::RegionCountMessage> message = std::make_shared<optriment::RegionCountMessage>();
        for (l_frame = batch_meta->frame_meta_list; l_frame != NULL; l_frame = l_frame->next) {
            NvDsFrameMeta *frame_meta = (NvDsFrameMeta *) (l_frame->data);
            std::stringstream out_string;
            for (NvDsMetaList * l_user = frame_meta->frame_user_meta_list; l_user != NULL; l_user = l_user->next) {
                NvDsUserMeta *user_meta = (NvDsUserMeta *) l_user->data;
                if (user_meta->base_meta.meta_type != NVDS_USER_FRAME_META_NVDSANALYTICS)
                    continue;

                NvDsAnalyticsFrameMeta *meta =
                    (NvDsAnalyticsFrameMeta *) user_meta->user_meta_data;
                for (std::pair<std::string, uint32_t> status : meta->objInROIcnt){
                    message->add(status.first, status.second);
                }
                publisher.publish(message);
            }
        }
        return GST_PAD_PROBE_OK;
}

static optriment::KafkaPublisher kafka_publisher("localhost:9092", "topic1");

int main (int argc, char *argv[])
{
    optriment::ArgParser arg_parser(argc, argv);
    char* file_path;
    std::string roi1_config;
    std::string roi2_config;
    if (!arg_parser.hasOption("--config") || !arg_parser.hasOption("--roi1") || !arg_parser.hasOption("--roi2")) {
        g_printerr("Usage: %s --config <config-file-path> --roi1 <roi1-config-file> --roi2 <roi2-config-file>\n", argv[0]);
        g_printerr("Using default config\n");
        file_path = "../config/pipeline_config.yml";
        roi1_config = "../config/roi1.txt";
        roi2_config = "../config/roi2.txt";
    } else {
        file_path = arg_parser.getOptionValue("--config");
        roi1_config = arg_parser.getOptionValue("--roi1");
        roi2_config = arg_parser.getOptionValue("--roi2");
    }
    initializer(argc, argv);
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
    .addElement("nveglglessink", "video-renderer");

    RETURN_ON_PARSER_ERROR(nvds_parse_gie_type(&pgie_type, file_path,
        "primary-gie"));
    RETURN_ON_PARSER_ERROR(nvds_parse_file_source(pipeline.getElement("file-source"), file_path, "source"));
    RETURN_ON_PARSER_ERROR(nvds_parse_streammux(pipeline.getElement("stream-muxer"), file_path, "streammux"));
    RETURN_ON_PARSER_ERROR(nvds_parse_gie(pipeline.getElement("pgie"), file_path, "primary-gie"));
    RETURN_ON_PARSER_ERROR(nvds_parse_tracker(pipeline.getElement("tracker"), file_path, "tracker"));


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
                           "on-screen-display", "video-renderer"});

    pipeline.attachProbe([](GstPad* pad, GstPadProbeInfo* info, gpointer u_data) {
        return probe(pad, info, u_data, kafka_publisher);
    }, "analytics2", "src");

    // TODO: Move to run().
    pipeline.run();
    return 0;
}
