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
		case SCENE_SINGLE_BOX:
			setupEnclosure(bodies, physicsGrid);
			setupSingleBoxScene(bodies, physicsGrid);
			break;
		case SCENE_VERTICAL_STACK:
			setupEnclosure(bodies, physicsGrid);
			setupVerticalStackScene(bodies, physicsGrid);
			break;
		case SCENE_COMPOUND_BODIES:
			setupEnclosure(bodies, physicsGrid);
			setupCompoundBodiesScene(bodies, physicsGrid);
			break;
		case SCENE_POLYGON_TEST:
			setupEnclosure(bodies, physicsGrid);
			setupPolygonTestScene(bodies, physicsGrid);
			break;
		case SCENE_SHAPES_TEST:
			setupEnclosure(bodies, physicsGrid);
			setupShapesTestScene(bodies, physicsGrid);
			break;
		case SCENE_RESTITUTION_TEST:
			setupEnclosure(bodies, physicsGrid);
			setupRestitutionTestScene(bodies, physicsGrid);
			break;
		case SCENE_BENCHMARK_CIRCLES:
			setupEnclosure(bodies, physicsGrid);
			setupCirclesBenchmarkScene(bodies, physicsGrid);
			break;
		case SCENE_BENCHMARK_BOXES:
			setupEnclosure(bodies, physicsGrid);
			setupBoxesBenchmarkScene(bodies, physicsGrid);
			break;
		case SCENE_BENCHMARK_POLYGONS:
			setupEnclosure(bodies, physicsGrid);
			setupPolygonBenchmarkScene(bodies, physicsGrid);
			break;
		case SCENE_BENCHMARK_ALL:
			setupEnclosure(bodies, physicsGrid);
			setupAllBenchmarkScene(bodies, physicsGrid);
			break;
	}

	m_currentScene = scene;
}

void SceneManager::setupEnclosure(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	// Create ground
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f, size.y - 10.0f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(size.x, 20.0f);
		boxDef.restitution = 1.0f;
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create right wall
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x - 10.0f, size.y * 0.5f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(20.0f, size.y);
		boxDef.restitution = 1.0f;
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create left wall
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(10.0f, size.y * 0.5f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(20.0f, size.y);
		boxDef.restitution = 1.0f;
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Create roof
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f, 10.0f);
		bodyDef.mass = 0.0f;
		bodyDef.inertia = 0.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(size.x, 20.0f);
		boxDef.restitution = 1.0f;
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupSingleBoxScene(list<Body*>& bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	BodyDef bodyDef;
	bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f, size.y * 0.5f);
	bodyDef.mass = 1.0f;
	bodyDef.inertia = 1.0f;

	BoxDef boxDef;
	boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
	bodyDef.shapes.push_back(new Box(boxDef));

	bodies.push_back(new Body(bodyDef, physicsGrid));
}

void SceneManager::setupVerticalStackScene(list<Body*>& bodies, PhysicsGrid * physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	for(int i = 0; i < 10; ++i)
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f, size.y - 50 - i * 55);
		bodyDef.mass = 1.0f;
		bodyDef.inertia = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupCompoundBodiesScene(list<Body*>& bodies, PhysicsGrid * physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	// Hammer
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f + 200.0f, size.y - 25 - 40);
		bodyDef.mass = 5.0f;
		bodyDef.inertia = 1.0f;
		bodyDef.angle = 0.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(200.0f, 20.0f);
		boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(200.0f * 0.5f - 20.0f  * 0.5f, 0.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		BoxDef boxDef2;
		boxDef2.size = Vector2FInPhysicsSpace(40.0f, 80.0f);
		boxDef2.bodyRelativePosition = Vector2FInPhysicsSpace(160.0f, 0.0f);
		bodyDef.shapes.push_back(new Box(boxDef2));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Seasaw
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f - 200.0f, size.y - 25 - 50);
		bodyDef.mass = 2.0f;
		bodyDef.inertia = 1.0f;
		bodyDef.angle = 0.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(400.0f, 20.0f);
		boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(0.0f, 0.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		CircleDef circleDef;
		circleDef.radius = ValueInPhysicsSpace(30.0f);
		circleDef.bodyRelativePosition = Vector2FInPhysicsSpace(0.0f, 0.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Box
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f - 100.0f, size.y - 100.0f);
		bodyDef.mass = 1.0f;
		bodyDef.inertia = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Spiral
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f - 300.0f, size.y * 0.5f);
		bodyDef.mass = 5.0f;
		bodyDef.inertia = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(100.0f, 10.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		BoxDef boxDef1;
		boxDef1.size = Vector2FInPhysicsSpace(80.0f, 10.0f);
		boxDef1.angle = 90.0f / 4.0f * 1;
		bodyDef.shapes.push_back(new Box(boxDef1));

		BoxDef boxDef2;
		boxDef2.size = Vector2FInPhysicsSpace(60.0f, 10.0f);
		boxDef2.angle = 90.0f / 4.0f * 2;
		bodyDef.shapes.push_back(new Box(boxDef2));

		BoxDef boxDef3;
		boxDef3.size = Vector2FInPhysicsSpace(40.0f, 10.0f);
		boxDef3.angle = 90.0f / 4.0f * 3;
		bodyDef.shapes.push_back(new Box(boxDef3));

		BoxDef boxDef4;
		boxDef4.size = Vector2FInPhysicsSpace(60.0f, 10.0f);
		boxDef4.angle = 90.0f / 4.0f * 4;
		bodyDef.shapes.push_back(new Box(boxDef4));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	// Mickey box
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f + 300.0f, size.y - 100.0f);
		bodyDef.mass = 3.0f;
		bodyDef.inertia = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		CircleDef circleDef;
		circleDef.radius = ValueInPhysicsSpace(15.0f);
		circleDef.bodyRelativePosition = Vector2FInPhysicsSpace(25.0f, 25.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		circleDef.bodyRelativePosition = Vector2FInPhysicsSpace(-25.0f, 25.0f);
		circleDef.bodyRelativeAngle = math::degToRad(45.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		boxDef.size = Vector2FInPhysicsSpace(30.0f, 20.0f);
		boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(25.0f, -25.0f);
		boxDef.bodyRelativeAngle = math::degToRad(45.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(-25.0f, -25.0f);
		boxDef.bodyRelativeAngle = -math::degToRad(45.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(size.x * 0.5f + 400.0f, size.y - 100.0f);
		bodyDef.mass = 2.0f;
		bodyDef.inertia = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		boxDef.angle = math::degToRad(45.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupPolygonTestScene(list<Body*>& bodies, PhysicsGrid * physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(150.0f, size.y - 150.0f);
		bodyDef.mass = bodyDef.inertia = 200.0f;

		PolygonShapeDef polygonDef;
		vector<Vector2F> vertices = {
			Vector2FInPhysicsSpace(-100.0f, -100.0f),
			Vector2FInPhysicsSpace(-50.0f, -120.0f),
			Vector2FInPhysicsSpace(+0.0f, -130.0f),
			Vector2FInPhysicsSpace(+80.0f, -90.0f),
			Vector2FInPhysicsSpace(+100.0f, 50.0f),
			Vector2FInPhysicsSpace(+0.0f, 100.0f),
			Vector2FInPhysicsSpace(-60.0f, 80.0f),
			Vector2FInPhysicsSpace(-90.0f, 0.0f),
		};
		polygonDef.vertices = vertices;
		bodyDef.shapes.push_back(new PolygonShape(polygonDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	for(int j = 0; j < 10; ++j)
	{
		for(int i = 0; i < 10 - j; ++i)
		{
			BodyDef bodyDef;
			bodyDef.position = Vector2FInPhysicsSpace(size.x - 150 - 55 * i - j * 25, size.y - 50 - 55.0f * j);
			bodyDef.mass = bodyDef.inertia = 1.0f;

			BoxDef boxDef;
			boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
			bodyDef.shapes.push_back(new Box(boxDef));

			bodies.push_back(new Body(bodyDef, physicsGrid));
		}
	}
}

void SceneManager::setupCirclesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	const float radiusMin = 1.0f;
	const float radiusMax = 20.0f;

	Random rand(g_randomSeed);
	for(int i = 0; i < g_benchmarkSceneNumCircles; ++i)
	{
		const float radius = ValueInPhysicsSpace(rand.nextDouble(radiusMin, radiusMax));
		const Vector2F spawnPoint = Vector2FInPhysicsSpace(rand.nextDouble(20.0f+radius, size.x-20.0f-radius), rand.nextDouble(20.0f+radius, size.x-20.0f-radius));

		BodyDef bodyDef;
		bodyDef.position = spawnPoint;

		CircleDef circleDef;
		circleDef.radius = radius;
		bodyDef.shapes.push_back(new Circle(circleDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}
}

void SceneManager::setupPolygonBenchmarkScene(list<Body*>& bodies, PhysicsGrid * physicsGrid)
{
}

void SceneManager::setupAllBenchmarkScene(list<Body*>& bodies, PhysicsGrid * physicsGrid)
{
}

void SceneManager::setupBoxesBenchmarkScene(list<Body*> &bodies, PhysicsGrid *physicsGrid)
{
	Window *window = Game::Get()->getWindow();
	const Vector2I size = window->getSize();

	const float radiusMin = 5.0f;
	const float radiusMax = 40.0f;

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

void SceneManager::setupShapesTestScene(list<Body*>& bodies, PhysicsGrid *physicsGrid)
{
	// Create a shape that tests compound bodies with body-relative transforms
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(100.0f, 600.0f);
		bodyDef.mass = 0.001f;
		bodyDef.inertia = 0.001f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		CircleDef circleDef;
		circleDef.radius = ValueInPhysicsSpace(15.0f);
		circleDef.bodyRelativePosition = Vector2FInPhysicsSpace(25.0f, 25.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		circleDef.bodyRelativePosition = Vector2FInPhysicsSpace(-25.0f, 25.0f);
		circleDef.bodyRelativeAngle = math::degToRad(45.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		boxDef.size = Vector2FInPhysicsSpace(30.0f, 20.0f);
		boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(25.0f, -25.0f);
		boxDef.bodyRelativeAngle = math::degToRad(45.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		boxDef.bodyRelativePosition = Vector2FInPhysicsSpace(-25.0f, -25.0f);
		boxDef.bodyRelativeAngle = -math::degToRad(75.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(600.0f, 600.0f);
		bodyDef.mass = 0.001f;
		bodyDef.inertia = 0.001f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		boxDef.angle = math::degToRad(45.0f);
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(500.0f, 600.0f);
		bodyDef.mass = 200.0f;
		bodyDef.inertia = 33.000f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		boxDef.friction = 1.0f;
		bodyDef.shapes.push_back(new Box(boxDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(300.0f, 600.0f);
		bodyDef.mass = 200.f;
		bodyDef.inertia = 33.0f;

		CircleDef circleDef;
		circleDef.radius = ValueInPhysicsSpace(25.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(300.0f, 600.0f);
		bodyDef.mass = 200.f;
		bodyDef.inertia = 33.0f;

		CircleDef circleDef;
		circleDef.radius = ValueInPhysicsSpace(25.0f);
		bodyDef.shapes.push_back(new Circle(circleDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	}

	for(int i = 0; i < 10; i++)
	{
		float yoffset = i * 55.0f;
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(800.0f, 600.0f - yoffset);
		bodyDef.mass = 1.0f;
		bodyDef.inertia = 1.0f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(50.0f, 50.0f);
		boxDef.friction = 1.0f;
		boxDef.restitution = 0.0f;
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

		CircleDef circleDef;
		circleDef.radius = ValueInPhysicsSpace(15.0f);
		circleDef.restitution = restitution;
		bodyDef.shapes.push_back(new Circle(circleDef));

		bodies.push_back(new Body(bodyDef, physicsGrid));
	};

	auto createBox = [&](float x, float restitution)
	{
		BodyDef bodyDef;
		bodyDef.position = Vector2FInPhysicsSpace(x, 100.0f);
		bodyDef.mass = 0.001f;
		bodyDef.inertia = 0.0001f;

		BoxDef boxDef;
		boxDef.size = Vector2FInPhysicsSpace(30.0f, 30.0f);
		boxDef.restitution = restitution;
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
