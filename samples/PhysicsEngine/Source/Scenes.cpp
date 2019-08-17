#include "Constants.h"
#include "Scenes.h"
#include "PhysicsGrid.h"

void SceneManager::initialize(const ExampleScene scene, list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	for(Body *body : bodies)
	{
		delete body;
	}
	bodies.clear();

	switch(scene)
	{
		case SCENE_EMPTY:
			break;
		case SCENE_ENCLOSURE:
			setupEnclosureScene(bodies, physicsGrid);
			break;
		case SCENE_SHAPES_TEST:
			setupEnclosureScene(bodies, physicsGrid);
			setupShapeTestScene(bodies, physicsGrid);
			break;
		case SCENE_RESTITUTION_TEST:
			setupEnclosureScene(bodies, physicsGrid);
			setupRestitutionTestScene(bodies, physicsGrid);
			break;
		case SCENE_BENCHMARK_CIRCLES:
			setupEnclosureScene(bodies, physicsGrid);
			setupCirclesBenchmarkScene(bodies, physicsGrid);
			break;
		case SCENE_BENCHMARK_BOXES:
			setupEnclosureScene(bodies, physicsGrid);
			setupBoxesBenchmarkScene(bodies, physicsGrid);
			break;
	}
}

void SceneManager::setupEnclosureScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	// Create ground
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f, size.y - 10.0f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(size.x, 20.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create right wall
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x - 10.0f, size.y * 0.5f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(20.0f, size.y);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create left wall
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(10.0f, size.y * 0.5f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(20.0f, size.y);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create roof
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f, 10.0f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(size.x, 20.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupCirclesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	const float radiusMin = 2.0f;
	const float radiusMax = 20.0f;

	Random rand(g_randomSeed);
	for(int i = 0; i < g_benchmarkSceneNumCircles; ++i)
	{
		const float radius = ValueInPhysicsSpace(rand.nextDouble(radiusMin, radiusMax));
		const Vector2F spawnPoint = Vector2FInPhysicsSpace(rand.nextDouble(20.0f+radius, size.x-20.0f-radius), rand.nextDouble(20.0f+radius, size.x-20.0f-radius));

		BodyDef bodyDef;
		bodyDef.position = spawnPoint;
		bodyDef.mass = 0.001f;

		CircleDef circleDef;
		circleDef.radius = radius;
		bodyDef.shapes.push_back(new Circle(circleDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupBoxesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	const float radiusMin = 20.0f;
	const float radiusMax = 50.0f;

	Random rand(g_randomSeed);
	for(int i = 0; i < g_benchmarkSceneNumBoxes; ++i)
	{
		const Vector2F boxSize = Vector2FInPhysicsSpace(rand.nextDouble(radiusMin, radiusMax), rand.nextDouble(radiusMin, radiusMax));
		const Vector2F spawnPoint = Vector2FInPhysicsSpace(rand.nextDouble(20.0f+boxSize.x, size.x-20.0f-boxSize.x), rand.nextDouble(20.0f+boxSize.y, size.x-20.0f-boxSize.y));

		BodyDef bodyDef;
		bodyDef.position = spawnPoint;
		bodyDef.angle = math::degToRad(rand.nextDouble(0.0f, 360.0f));
		bodyDef.mass = 0.001f;

		BoxDef boxDef;
		boxDef.size = boxSize;
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupShapeTestScene(list<Body*>& bodies, PhysicsGrid *physicsGrid)
{
	// Create a shape that tests compound bodies with body-relative transforms
	//{
	//	BodyDef bodyDef;
	//	bodyDef.position = Vector2FInPhysicsSpace(100.0f, 600.0f);
	//	bodyDef.mass = 0.001f;
	//	bodyDef.inertia = 0.001f;

	//	BoxDef boxDef;
	//	boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
	//	bodyDef.shapes.push_back(new Box(boxDef));

	//	CircleDef circleDef;
	//	circleDef.radius = ValueInPhysicsSpace(15.0f);
	//	circleDef.bodyRelativePosition = Vector2FInPhysicsSpace(25.0f, 25.0f);
	//	bodyDef.shapes.push_back(new Circle(circleDef));

	//	circleDef.bodyRelativePosition = Vector2FInPhysicsSpace(-25.0f, 25.0f);
	//	circleDef.bodyRelativeAngle = math::degToRad(45.0f);
	//	bodyDef.shapes.push_back(new Circle(circleDef));

	//	boxDef.size = Vector2FInPhysicsSpace(30.0f, 20.0f);
	//	boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(25.0f, -25.0f);
	//	boxDef.bodyRelativeAngle = math::degToRad(45.0f);
	//	bodyDef.shapes.push_back(new Box(boxDef));

	//	boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(-25.0f, -25.0f);
	//	boxDef.bodyRelativeAngle = -math::degToRad(75.0f);
	//	bodyDef.shapes.push_back(new Box(boxDef));

	//	bodies.push_back(new Body(bodyDef, physicsGrid));
	//}

	//{
	//	BodyDef bodyDef;
	//	bodyDef.position = Vector2FInPhysicsSpace(600.0f, 600.0f);
	//	bodyDef.mass = 0.001f;
	//	bodyDef.inertia = 0.001f;

	//	BoxDef boxDef;
	//	boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
	//	bodyDef.shapes.push_back(new Box(boxDef));

	//	boxDef.angle = math::degToRad(45.0f);
	//	bodyDef.shapes.push_back(new Box(boxDef));

	//	bodies.push_back(new Body(bodyDef, physicsGrid));
	//}

	//{
	//	BodyDef bodyDef;
	//	bodyDef.position = Vector2FInPhysicsSpace(500.0f, 600.0f);
	//	bodyDef.mass = 200.0f;
	//	bodyDef.inertia = 33.000f;
	//	bodyDef.friction = 1.0f;

	//	BoxDef boxDef;
	//	boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
	//	bodyDef.shapes.push_back(new Box(boxDef));

	//	bodies.push_back(new Body(bodyDef, physicsGrid));
	//}

	//{
	//	BodyDef bodyDef;
	//	bodyDef.position = Vector2FInPhysicsSpace(300.0f, 600.0f);
	//	bodyDef.mass = 200.f;
	//	bodyDef.inertia = 33.0f;

	//	CircleDef circleDef;
	//	circleDef.radius = ValueInPhysicsSpace(25.0f);
	//	bodyDef.shapes.push_back(new Circle(circleDef));

	//	bodies.push_back(new Body(bodyDef, physicsGrid));
	//}

	//{
	//	BodyDef bodyDef;
	//	bodyDef.position = Vector2FInPhysicsSpace(300.0f, 600.0f);
	//	bodyDef.mass = 200.f;
	//	bodyDef.inertia = 33.0f;

	//	CircleDef circleDef;
	//	circleDef.radius = ValueInPhysicsSpace(25.0f);
	//	bodyDef.shapes.push_back(new Circle(circleDef));

	//	bodies.push_back(new Body(bodyDef, physicsGrid));
	//}

	for(int i = 0; i < 10; i++)
	{
		float yoffset = i * 55.0f;
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(800.0f, 600.0f - yoffset);
		bodyDef.mass = 1.0f;
		bodyDef.inertia = 1.0f;
		bodyDef.friction = 1.0f;
		bodyDef.restitution = 0.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupRestitutionTestScene(list<Body*>& bodies, PhysicsGrid * physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	auto createCircle = [&](float x, float restitution)
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(x, 100.0f);
		bodyDef.mass = 0.001f;
		bodyDef.inertia = 0.0001f;
		bodyDef.restitution = restitution;

		CircleDef circleDef;
		circleDef.radius = ValueInPhysicsSpace(15.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	};

	auto createBox = [&](float x, float restitution)
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(x, 100.0f);
		bodyDef.mass = 0.001f;
		bodyDef.inertia = 0.0001f;
		bodyDef.restitution = restitution;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(30.0f, 30.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	};

	for(float restitution = 0.0f, x = 100.0f; restitution < 1.0f; restitution += 0.1f, x += 35.0f)
	{
		createCircle(x, restitution);
	}

	for(float restitution = 0.0f, x = size.x - 100.0f; restitution < 1.0f; restitution += 0.1f, x -= 35.0f)
	{
		createBox(x, restitution);
	}
}
