#include "Scenes.h"
#include "Config.h"
#include "PhysicsGrid.h"


void PhysicsScene::initialize(const ExampleScene scene, list<Body*> &bodies, PhysicsGrid *physicsGrid)
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

void PhysicsScene::setupEnclosureScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	// Create ground
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2F(size.x * 0.5f, size.y - 10.0f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		Box *box = new Box;
		box->setSize(Vector2F(size.x, 20.0f));
		bodyDef.shapes.push_back(box);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create right wall
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2F(size.x - 10.0f, size.y * 0.5f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		Box *box = new Box;
		box->setSize(Vector2F(20.0f, size.y));
		bodyDef.shapes.push_back(box);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create left wall
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2F(10.0f, size.y * 0.5f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		Box *box = new Box;
		box->setSize(Vector2F(20.0f, size.y));
		bodyDef.shapes.push_back(box);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create roof
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2F(size.x * 0.5f, 10.0f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;
		bodyDef.restitution = 1.0f;

		Box *box = new Box;
		box->setSize(Vector2F(size.x, 20.0f));
		bodyDef.shapes.push_back(box);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void PhysicsScene::setupCirclesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	const float radiusMin = 2.0f;
	const float radiusMax = 20.0f;

	Random rand(g_randomSeed);
	for(int i = 0; i < g_benchmarkSceneNumCircles; ++i)
	{
		const float radius = rand.nextDouble(radiusMin, radiusMax);
		const Vector2F spawnPoint = Vector2F(rand.nextDouble(20.0f+radius, size.x-20.0f-radius), rand.nextDouble(20.0f+radius, size.x-20.0f-radius));

		BodyDef bodyDef;
		bodyDef.position = spawnPoint;
		bodyDef.mass = 0.001f;

		Circle *circle = new Circle;
		circle->setRadius(radius);
		bodyDef.shapes.push_back(circle);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void PhysicsScene::setupBoxesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	const float radiusMin = 20.0f;
	const float radiusMax = 50.0f;

	Random rand(g_randomSeed);
	for(int i = 0; i < g_benchmarkSceneNumBoxes; ++i)
	{
		const Vector2F boxSize = Vector2F(rand.nextDouble(radiusMin, radiusMax), rand.nextDouble(radiusMin, radiusMax));
		const Vector2F spawnPoint = Vector2F(rand.nextDouble(20.0f+boxSize.x, size.x-20.0f-boxSize.x), rand.nextDouble(20.0f+boxSize.y, size.x-20.0f-boxSize.y));

		BodyDef bodyDef;
		bodyDef.position = spawnPoint;
		bodyDef.angle = math::degToRad(rand.nextDouble(0.0f, 360.0f));
		bodyDef.mass = 0.001f;

		Box *box = new Box;
		box->setSize(boxSize);
		bodyDef.shapes.push_back(box);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void PhysicsScene::setupShapeTestScene(list<Body*>& bodies, PhysicsGrid *physicsGrid)
{
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2F(100.0f, 600.0f);
		bodyDef.mass = 0.001f;

		Box *box = new Box;
		box->setSize(Vector2F(50.0f));
		bodyDef.shapes.push_back(box);

		Circle *circle = new Circle;
		circle->setRadius(15.0f);
		circle->setLocalPosition(Vector2F(25.0f, 25.0f));
		bodyDef.shapes.push_back(circle);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void PhysicsScene::setupRestitutionTestScene(list<Body*>& bodies, PhysicsGrid * physicsGrid)
{
	auto createCircle = [&](float offsetX, float restitution)
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2F(100.0f + offsetX, 100.0f);
		bodyDef.mass = 0.001f;
		bodyDef.restitution = restitution;

		Circle *circle = new Circle;
		circle->setRadius(15.0f);
		bodyDef.shapes.push_back(circle);

		//Box *box = new Box;
		//box->setSize(Vector2F(30.0f, 30.0f));
		//bodyDef.shapes.push_back(box);

		bodies.push_back(new Body(bodyDef, physicsGrid));
	};

	createCircle(0.0f, 1.0f);
	createCircle(50.0f, 0.9f);
	createCircle(100.0f, 0.8f);
	createCircle(150.0f, 0.7f);
	createCircle(200.0f, 0.6f);
	createCircle(250.0f, 0.5f);
	createCircle(300.0f, 0.4f);
	createCircle(350.0f, 0.3f);
	createCircle(400.0f, 0.2f);
	createCircle(450.0f, 0.1f);
	createCircle(500.0f, 0.0f);
}
