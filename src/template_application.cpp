#include "../include/template_application.h"

#include "../include/replay_file.h"

#include <sstream>
#include <string>

TemplateApplication::TemplateApplication() {
    m_frame = 0;
    m_cameraPosition = ysMath::LoadVector(0.0f, 0.0f, 10.0f);
    m_replayFile = nullptr;
}

TemplateApplication::~TemplateApplication() {
    /* void */
}

void TemplateApplication::Initialize(void *instance, ysContextObject::DEVICE_API api) {
    dbasic::Path modulePath = dbasic::GetModulePath();
    dbasic::Path confPath = modulePath.Append("delta.conf");

    std::string enginePath = "../../dependencies/delta/engines/basic";
    std::string assetPath = "../../assets";
    if (confPath.Exists()) {
        std::fstream confFile(confPath.ToString(), std::ios::in);
        
        std::getline(confFile, enginePath);
        std::getline(confFile, assetPath);
        enginePath = modulePath.Append(enginePath).ToString();
        assetPath = modulePath.Append(assetPath).ToString();

        confFile.close();
    }

    m_engine.GetConsole()->SetDefaultFontDirectory(enginePath + "/fonts/");

    m_engine.CreateGameWindow(
        "Delta Physics Replay",
        instance,
        api,
        (enginePath + "/shaders/").c_str());

    m_engine.SetClearColor(0x0, 0x0, 0x0);

    m_assetManager.SetEngine(&m_engine);

    m_assetManager.CompileInterchangeFile((assetPath + "/circle").c_str(), 1.0f, true);
    m_assetManager.LoadSceneFile((assetPath + "/circle").c_str());

    CircleBounds::m_circleGeometry = m_assetManager.GetModelAsset("Circle");

    //m_replayFile = new ReplayFile("E:\\Software\\ant-world\\dependencies\\submodules\\delta-studios\\project\\Debug\\SystemTest_replay.txt");
    m_replayFile = new ReplayFile("D:\\Projects\\Software\\ant-world\\dependencies\\submodules\\delta-studios\\project\\x64\\Debug\\SystemTest_replay.txt");

    m_cameraPosition = ysMath::LoadVector(0.0f, 0.0f, 10.0f);
}

void TemplateApplication::Process() {
    if (m_engine.ProcessKeyDown(ysKeyboard::KEY_LEFT)) {
        --m_frame;
    }
    else if (m_engine.ProcessKeyDown(ysKeyboard::KEY_RIGHT)) {
        ++m_frame;
    }

    if (m_frame < 0) m_frame = 0;
    if (m_frame >= m_replayFile->getFrameCount()) m_frame = m_replayFile->getFrameCount() - 1;
}

void TemplateApplication::Render() {
    m_engine.SetCameraPosition(ysMath::GetX(m_cameraPosition), ysMath::GetY(m_cameraPosition));
    m_engine.SetCameraAltitude(ysMath::GetZ(m_cameraPosition));
    
    m_engine.m_lightingControls.AmbientLighting = ysVector4(1.0f, 1.0f, 1.0f, 1.0f);

    m_replayFile->render(&m_engine, m_frame);

    std::stringstream ss;
    ss << "F" << m_frame + 1 << "/" << m_replayFile->getFrameCount();

    m_engine.GetConsole()->Clear();
    m_engine.GetConsole()->MoveToLocation(dbasic::GuiPoint(1, 2));
    m_engine.GetConsole()->SetFontForeColor(0, 0, 0, 1.0f);
    m_engine.GetConsole()->SetFontBackColor(0, 0, 0, 0.0f);
    m_engine.GetConsole()->DrawGeneralText(ss.str().c_str());
}

void TemplateApplication::Run() {
    while (m_engine.IsOpen()) {
        m_engine.StartFrame();

        Process();
        Render();

        m_engine.EndFrame();
    }

    m_engine.Destroy();
}
