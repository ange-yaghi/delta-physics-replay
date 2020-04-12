#ifndef DELTA_TEMPLATE_TEMPLATE_APPLICATION_H
#define DELTA_TEMPLATE_TEMPLATE_APPLICATION_H

#include "delta.h"

class ReplayFile;

class TemplateApplication {
public:
    TemplateApplication();
    ~TemplateApplication();

    void Initialize(void *instance, ysContextObject::DEVICE_API api);
    void Run();

protected:
    void Process();
    void Render();

    int m_frame;

    dbasic::DeltaEngine m_engine;
    dbasic::AssetManager m_assetManager;

    ysTexture *m_demoTexture;
    ReplayFile *m_replayFile;

    ysVector m_cameraPosition;
};

#endif /* DELTA_TEMPLATE_TEMPLATE_APPLICATION_H */
