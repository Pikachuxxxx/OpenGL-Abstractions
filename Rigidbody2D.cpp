#include "Rigidbody2D.h"

// std::shared_ptr<b2World> Rigidbody2D::s_World;

Rigidbody2D::Rigidbody2D(glm::vec3 position, glm::vec2 dimension, float density, float friction, Rigidbody2DType bodytype, b2World& world)
    : bodyType(bodytype), fixedRotation(false), gravityScale(1.0f), restitution(0), m_PhysicsTick(0.0166f), m_PositionIterations(2), m_VelocityIterations(6)
{
    // s_World = std::make_shared<b2World>(GRAVITY);

    world.SetContinuousPhysics(true);
    m_BodyDef.type = (b2BodyType)bodytype;
    m_BodyDef.position.Set(position.x * P2Mx, position.y * P2My);
    m_Body = world.CreateBody(&m_BodyDef);
    m_Body->SetGravityScale(gravityScale);
    m_Body->SetFixedRotation(fixedRotation);
    m_Shape.SetAsBox(dimension.x * P2Mx, dimension.y * P2My);
    m_Fixture.shape = &m_Shape;
    m_Fixture.density = density;
    m_Fixture.friction = friction;
    m_Fixture.restitution = restitution;
    m_Body->CreateFixture(&m_Fixture);
}

Rigidbody2D::~Rigidbody2D()
{
    // PhysicsWorld.DestroyBody(m_Body);
}

glm::vec3 Rigidbody2D::GetPositionInMeters()
{
    b2Vec2 pos = m_Body->GetPosition();
	return glm::vec3(pos.x, pos.y, 0.0f);
}

glm::vec3 Rigidbody2D::GetPositionInPixels()
{
    b2Vec2 pos = m_Body->GetPosition();
	return glm::vec3(pos.x * M2Px, pos.y * M2Py, 0.0f);
}

float Rigidbody2D::GetRotation()
{
    return m_Body->GetAngle();
}

void Rigidbody2D::AddForce(glm::vec2 direction, float force)
{
    m_Body->ApplyForce(b2Vec2(direction.x * force, direction.y * force), m_Body->GetWorldCenter(), true);
}

void Rigidbody2D::SetVelocity(glm::vec2 velocity)
{
    m_Body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y));
}
