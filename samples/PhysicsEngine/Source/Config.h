#pragma once

#include "Scenes.h"

//#define DEBUG_DRAW

#define DRAW_PHYSICS_GRID 0
#define DRAW_NORMALS 0
#define DRAW_IMPULSES 0
#define DRAW_AABB 1

#define DISABLE_COLLISIONS 0
#define RUN_AS_BENCHMARK 0

const uint32_t g_randomSeed = 453423120;

const PhysicsScene::ExampleScene g_initialScene = PhysicsScene::SCENE_SHAPES_TEST;//PhysicsScene::SCENE_ENCLOSURE;
const Vector2F                   g_defaultGravity = Vector2F(0.0f, 150.0f);

const Vector2I g_physicsCellSize      = Vector2I(200, 200); // px
const int      g_initialBodiesPerCell = 100;

// Benchmark constants                       // Current best (on release, with FPS ~30)
const int g_benchmarkSceneNumCircles = 1500; // ~1500
const int g_benchmarkSceneNumBoxes   = 1500; // ~1000
