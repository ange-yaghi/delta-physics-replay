#ifndef PHYSICS_REPLAY_REPLAY_FILE_H
#define PHYSICS_REPLAY_REPLAY_FILE_H

#include "delta.h"

#include <string>

class Bounds {
public:
    Bounds();
    ~Bounds();

    virtual void render(dbasic::DeltaEngine *engine, ysTransform &space) = 0;
};

class BoxBounds : public Bounds {
public:
    BoxBounds();
    ~BoxBounds();

    virtual void render(dbasic::DeltaEngine *engine, ysTransform &space);

    float m_halfWidth;
    float m_halfHeight;
    ysVector m_position;
    ysQuaternion m_orientation;
};

class CircleBounds : public Bounds {
public:
    CircleBounds();
    ~CircleBounds();

    virtual void render(dbasic::DeltaEngine *engine, ysTransform &space);

    float m_radius;
    ysVector m_position;
};

class RigidBodyProxy {
public:
    RigidBodyProxy();
    ~RigidBodyProxy();

    void render(dbasic::DeltaEngine *engine, int frame);

    std::vector<Bounds *> m_bounds;
    std::vector<ysTransform> m_transforms;
};

class ReplayFile {
public:
    ReplayFile(const std::string &fname);
    ~ReplayFile();

    void render(dbasic::DeltaEngine *engine, int frame);

    int getFrameCount() const { return m_frameCount; }

protected:
    std::vector<RigidBodyProxy *> m_bodies;
    int m_frameCount;

    void readFile(const std::string &fname);
};

#endif /* PHYSICS_REPLAY_REPLAY_FILE_H */
