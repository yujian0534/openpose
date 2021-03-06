#ifndef OPENPOSE_POSE_W_POSE_RENDERER_HPP
#define OPENPOSE_POSE_W_POSE_RENDERER_HPP

#include <memory> // std::shared_ptr
#include <openpose/thread/worker.hpp>
#include "poseRenderer.hpp"

namespace op
{
    template<typename TDatums>
    class WPoseRenderer : public Worker<TDatums>
    {
    public:
        explicit WPoseRenderer(const std::shared_ptr<PoseRenderer>& poseRendererSharedPtr);

        void initializationOnThread();

        void work(TDatums& tDatums);

    private:
        std::shared_ptr<PoseRenderer> spPoseRenderer;

        DELETE_COPY(WPoseRenderer);
    };
}





// Implementation
#include <openpose/utilities/errorAndLog.hpp>
#include <openpose/core/macros.hpp>
#include <openpose/utilities/pointerContainer.hpp>
#include <openpose/utilities/profiler.hpp>
namespace op
{
    template<typename TDatums>
    WPoseRenderer<TDatums>::WPoseRenderer(const std::shared_ptr<PoseRenderer>& poseRendererSharedPtr) :
        spPoseRenderer{poseRendererSharedPtr}
    {
    }

    template<typename TDatums>
    void WPoseRenderer<TDatums>::initializationOnThread()
    {
        spPoseRenderer->initializationOnThread();
    }

    template<typename TDatums>
    void WPoseRenderer<TDatums>::work(TDatums& tDatums)
    {
        try
        {
            if (checkNoNullNorEmpty(tDatums))
            {
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
                // Profiling speed
                const auto profilerKey = Profiler::timerInit(__LINE__, __FUNCTION__, __FILE__);
                // Render people pose
                for (auto& tDatum : *tDatums)
                    tDatum.elementRendered = spPoseRenderer->renderPose(tDatum.outputData, tDatum.poseKeypoints, (float)tDatum.scaleNetToOutput);
                // Profiling speed
                Profiler::timerEnd(profilerKey);
                Profiler::printAveragedTimeMsOnIterationX(profilerKey, __LINE__, __FUNCTION__, __FILE__, Profiler::DEFAULT_X);
                // Debugging log
                dLog("", Priority::Low, __LINE__, __FUNCTION__, __FILE__);
            }
        }
        catch (const std::exception& e)
        {
            this->stop();
            tDatums = nullptr;
            error(e.what(), __LINE__, __FUNCTION__, __FILE__);
        }
    }

    COMPILE_TEMPLATE_DATUM(WPoseRenderer);
}

#endif // OPENPOSE_POSE_W_POSE_RENDERER_HPP
