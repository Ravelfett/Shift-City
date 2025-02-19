#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <raylib.h>

#include "structures.h"
#include "drawing.h"
#include "input_management.h"
#include "city_manipulation.h"
#include "objectives.h"



void Init(Color backgroundColor) {
  SetConfigFlags(FLAG_WINDOW_TOPMOST || FLAG_WINDOW_RESIZABLE);
  InitWindow(0, 0, "Shift City");
  ClearBackground(backgroundColor);
  SetTargetFPS(60);
}



int main() {
  // init
  Color backgroundColor = (Color) {100, 100, 100, 255};
  Color objectivesBackgroundColor = (Color) {100, 100, 100, 255};
  Init(backgroundColor);
  ToggleFullscreen();

  // camera control
  float pi = 3.141592;
  float verticalAngle = pi / 6.;
  float horizontalAngle = pi;
  float targetDistance = 4.;
  float speed = 0.1;
  Camera3D camera = {0};
  camera.position = (Vector3) {0.0f, 0.0f, 0.0f};
  camera.target = (Vector3) {0.0f, 0.0f, 1.0f};
  camera.up = (Vector3) {0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
  camera.fovy = 40.0f; // Camera field-of-view Y
  camera.projection = CAMERA_PERSPECTIVE;

  // objectives
  int objectiveView = 0; //if 1 then enter objective view mode
  Camera3D objectivesCamera = {0};
  objectivesCamera.position = (Vector3) {11.0f, 2.0f, 1.0f};
  objectivesCamera.target = (Vector3) {9.0f, 0.0f, -1.0f};
  objectivesCamera.up = (Vector3) {0.0f, 1.0f, 0.0f}; // Camera up vector (rotation towards target)
  objectivesCamera.fovy = 10.0f; // Camera field-of-view Y
  objectivesCamera.projection = CAMERA_ORTHOGRAPHIC;
  int points = 0;
  AllObjectives *allObjectives = BuildAllObjectives();

  // cursor control
  int cursorTileX = 0;
  int cursorTileZ = 0;

  // initialize space
  Building *stash = makeBuilding(0, 0, 0);
  City *city1 = buildWholeCity();
  int maxStashSize = 1;
  
  while (!WindowShouldClose()) {
    // change objective view
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      int mouseX = GetMouseX();
      int mouseY = GetMouseY();
      if (mouseX > 20 && mouseX < 200 && mouseY > 20 && mouseY < 65) objectiveView = (objectiveView + 1) % 2;
    }
    if (IsKeyPressed(KEY_TAB)) {
      objectiveView = (objectiveView + 1) % 2;
    }
    if (objectiveView == 0) {
      // input
      updateCamera(&camera, pi, speed, &verticalAngle, &horizontalAngle, &targetDistance);
      updateCursorBuildingCoordinates(camera, city1, &cursorTileX, &cursorTileZ);
      if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        stash = stashFloor(city1, cursorTileX, cursorTileZ, stash, maxStashSize);
      }
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Building *newStash = dropFloor(city1, cursorTileX, cursorTileZ, stash);
        // check if a floor was dropped
        if (newStash != stash) {
          checkObjectiveCompletion(city1->buildings[findBuildingNb(city1, cursorTileX, cursorTileZ)], allObjectives, &points);
        }
        stash = newStash;
      }

      // drawing
      BeginDrawing();
        ClearBackground(backgroundColor);
        BeginMode3D(camera);
          drawCity(city1, cursorTileX, cursorTileZ);
          drawStash(stash, cursorTileX, cursorTileZ);
        EndMode3D();
        drawPoints(points);
        drawObjectiveButton();
      EndDrawing();
    }
    else {
      BeginDrawing();
        ClearBackground(objectivesBackgroundColor);
        BeginMode3D(objectivesCamera);
          drawAllObjectives(allObjectives);
        EndMode3D();
        drawPoints(points);
        drawObjectiveButton();
      EndDrawing();
    }
  }
  CloseWindow();

  // de-init
  freeCity(city1);
  freeBuilding(stash);
  freeAllObjectives(allObjectives);

  return 0;
}
