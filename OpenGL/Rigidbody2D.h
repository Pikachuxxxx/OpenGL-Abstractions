// #include <memory>
//
// // Box2D
// #include <box2d/box2d.h>
// // GLM
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
//
// static double M2Px = 32.0f, M2Py = 24.0f;
// static double P2Mx = 1.0f / M2Px, P2My = 1.0f / M2Py;
//
// static const b2Vec2 GRAVITY(0.0f, -9.81f);
// static b2World PhysicsWorld(GRAVITY);
//
// enum Rigidbody2DType
// {
//     Static = b2_staticBody,
//     Dynamic = b2_dynamicBody,
//     Kinematic = b2_kinematicBody
// };
//
// class Rigidbody2D
// {
// public:
//     Rigidbody2DType						bodyType;
//     float                               gravityScale;
//     float                               restitution;
//     bool								fixedRotation;
// private:
//     // static std::shared_ptr<b2World>	    s_World;
//
//     float                               m_PhysicsTick;
//     int                                 m_VelocityIterations;
//     int                                 m_PositionIterations;
//     b2BodyDef							m_BodyDef;
//     b2Body*								m_Body;
//     b2PolygonShape						m_Shape;
//     b2FixtureDef						m_Fixture;
//
//     bool								m_DidGenerateRB;
// public:
//
//     Rigidbody2D(glm::vec3 position, glm::vec2 dimensions, float density = 1.0f, float friction = 0.3f, Rigidbody2DType bodytype = Static, b2World& world = PhysicsWorld);
//     ~Rigidbody2D();
//
//     glm::vec3 GetPositionInMeters();
//     glm::vec3 GetPositionInPixels();
//     float GetRotation();
//
//     void AddForce(glm::vec2 direction, float force);
//     void SetVelocity(glm::vec2 velocity);
//
//     inline b2Body* GetBody() { return  m_Body; }
// };
