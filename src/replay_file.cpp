#include "../include/replay_file.h"

Bounds::Bounds() {
    /* void */
}

Bounds::~Bounds() {
    /* void */
}

RigidBodyProxy::RigidBodyProxy() {
    /* void */
}

RigidBodyProxy::~RigidBodyProxy() {
    /* void */
}

void RigidBodyProxy::render(dbasic::DeltaEngine *engine, int frame) {
    ysTransform &transform = m_transforms[frame];

    for (Bounds *bounds : m_bounds) {
        bounds->render(engine, transform, m_color);
    }
}

ReplayFile::ReplayFile(const std::string &fname) {
    m_frameCount = 0;

    m_colors = new ysVector4[5];
    m_colors[0] = ysVector4(254 / 255.0f, 0.0f, 0.0f, 1.0f);
    m_colors[1] = ysVector4(253 / 255.0f, 254 / 255.0f, 2 / 255.0f, 1.0f);
    m_colors[2] = ysVector4(11 / 255.0f, 255 / 255.0f, 1 / 255.0f, 1.0f);
    m_colors[3] = ysVector4(1 / 255.0f, 30 / 255.0f, 254 / 255.0f, 1.0f);
    m_colors[4] = ysVector4(254 / 255.0f, 0 / 255.0f, 246 / 255.0f, 1.0f);

    readFile(fname);
}

ReplayFile::~ReplayFile() {
    /* void */
}

void ReplayFile::render(dbasic::DeltaEngine *engine, int frame) {
    for (RigidBodyProxy *body : m_bodies) {
        body->render(engine, frame);
    }
}

void ReplayFile::readFile(const std::string &fname) {
    std::fstream f(fname, std::ios::in);

    // Read in all rigid bodies
    std::string buffer;
    f >> buffer; // <Objects>

    int rigidBodyCount = 0;

    while (true) {
        f >> buffer;
        if (buffer == "</Objects>") break;

        RigidBodyProxy *newRigidBody = new RigidBodyProxy;
        newRigidBody->m_color = m_colors[rigidBodyCount % 5];
        m_bodies.push_back(newRigidBody);
        ++rigidBodyCount;
        
        while (true) {
            f >> buffer;
            if (buffer == "</RigidBody>") break;
            if (buffer == "<CollisionObject>") {
                std::string type;
                f >> buffer;
                f >> type;

                if (type == "CIRCLE") {
                    CircleBounds *newBounds = new(_aligned_malloc(sizeof(CircleBounds), 16)) CircleBounds;
                    newRigidBody->m_bounds.push_back(newBounds);

                    f >> buffer; // RADIUS
                    f >> newBounds->m_radius;

                    float x, y, z;
                    f >> buffer; // POSITION
                    f >> x;
                    f >> y;
                    f >> z;

                    newBounds->m_position = ysMath::LoadVector(x, y, z, 0.0f);
                }
                else if (type == "BOX") {
                    BoxBounds *newBounds = new(_aligned_malloc(sizeof(BoxBounds), 16)) BoxBounds;
                    newRigidBody->m_bounds.push_back(newBounds);

                    float halfWidth, halfHeight;
                    f >> buffer; // HALF_WIDTH
                    f >> halfWidth;

                    f >> buffer; // HALF_HEIGHT
                    f >> halfHeight;

                    float x, y, z;
                    float qw, qx, qy, qz;

                    f >> buffer; // POSITION
                    f >> x;
                    f >> y;
                    f >> z;
                    
                    f >> buffer; // ORIENTATION
                    f >> qw;
                    f >> qx;
                    f >> qy;
                    f >> qz;

                    newBounds->m_halfHeight = halfHeight;
                    newBounds->m_halfWidth = halfWidth;
                    newBounds->m_orientation = ysMath::LoadVector(qw, qx, qy, qz);
                    newBounds->m_position = ysMath::LoadVector(x, y, z);
                }

                f >> buffer; // </CollisionObject>
            }
            else break;
        }
    }

    while (true) {
        f >> buffer;
        if (f.eof()) break;
        if (buffer == "<Frame>") {
            ++m_frameCount;

            int rigidBodyCount = m_bodies.size();
            for (int i = 0; i < rigidBodyCount; ++i) {
                f >> buffer; // <Body>

                RigidBodyProxy *body = m_bodies[i];
                ysTransform newTransform;

                float x, y, z;
                float qw, qx, qy, qz;

                f >> buffer; // POSITION
                f >> x;
                f >> y;
                f >> z;

                f >> buffer; // ORIENTATION
                f >> qw;
                f >> qx;
                f >> qy;
                f >> qz;

                newTransform.SetOrientation(ysMath::LoadVector(qw, qx, qy, qz));
                newTransform.SetPosition(ysMath::LoadVector(x, y, z, 0.0f));

                body->m_transforms.push_back(newTransform);

                f >> buffer; // </Body>
            }

            f >> buffer; // </Frame>
        }
        else break;
    }

    f.close();
}

BoxBounds::BoxBounds() {
    m_halfHeight = 0;
    m_halfWidth = 0;
    m_orientation = ysMath::Constants::QuatIdentity;
    m_position = ysMath::Constants::Zero;
}

BoxBounds::~BoxBounds() {
    /* void */
}

void BoxBounds::render(dbasic::DeltaEngine *engine, ysTransform &space, const ysVector4 &color) {
    int rgb[] = { color.x * 255, color.y * 255, color.z * 255 };

    engine->SetObjectTransform(space.GetWorldTransform());
    engine->DrawBox(rgb, m_halfWidth * 2, m_halfHeight * 2, 0);
}

dbasic::ModelAsset *CircleBounds::m_circleGeometry = nullptr;

CircleBounds::CircleBounds() {
    m_radius = 0.0f;
    m_position = ysMath::Constants::Zero;
}

CircleBounds::~CircleBounds() {
    /* void */
}

void CircleBounds::render(dbasic::DeltaEngine *engine, ysTransform &space, const ysVector4 &color) {
    constexpr int line[] = { 255, 255, 255 };

    engine->DrawAxis(line, 
        space.LocalToWorldSpace(ysMath::LoadVector(0.0f, -m_radius, 0.0f)), 
        space.LocalToWorldDirection(ysMath::Constants::YAxis), 
        0.01f, m_radius * 2.0f, 1);
    engine->DrawAxis(line, 
        space.LocalToWorldSpace(ysMath::LoadVector(-m_radius, 0.0f, 0.0f)),
        space.LocalToWorldDirection(ysMath::Constants::XAxis), 
        0.01f, m_radius * 2.0f, 1);

    engine->SetMultiplyColor(color);
    engine->DrawModel(m_circleGeometry, space.GetWorldTransform(), m_radius, nullptr);
}
