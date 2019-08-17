#pragma once

#include "Scenes.h"

#define DRAW_POLYGON_TO_POLYGON_DEBUG_INFO 0
#define DRAW_PHYSICS_GRID 0
#define DRAW_NORMALS 0
#define DRAW_VELOCITIES 0
#define DRAW_IMPULSES 0
#define DRAW_AABB 0

//#define COLLIDED_POLYGON_COLOR Color::Blue
#define COLLIDED_POLYGON_COLOR Color::White

#define DISABLE_COLLISIONS 0
#define RUN_AS_BENCHMARK 0

const uint32_t g_randomSeed = 453423120;
const float    g_physicsUnit = 50.0f;
const float    g_physicsEpsilon = 0.0001f;
const double   g_deltaTime = 1.0 / 60.0;
const bool     g_accumulateImpulses = true;

const bool     g_positionCorrection = true;
const int      g_iterations = 10;

const SceneManager::ExampleScene g_initialScene = SceneManager::SCENE_BENCHMARK_BOXES;//PhysicsScene::SCENE_ENCLOSURE;
const Vector2F                   g_defaultGravity = Vector2F(0.0f, 150.0f) / g_physicsUnit;

const Vector2I g_physicsCellSize      = Vector2I(200, 200) / g_physicsUnit;
const int      g_initialBodiesPerCell = 100;

// Benchmark constants                       // Current best (on release, with FPS ~30)
const int g_benchmarkSceneNumCircles = 1500; // ~1500
const int g_benchmarkSceneNumBoxes   = 1500/2; // ~1000
