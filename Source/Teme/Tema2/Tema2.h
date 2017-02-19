/*Mustata Alexandru-Ionut
*Grupa: 331CB
*Tema 2 EGC: Build and Survive
*/

#pragma once
#include <Component/SimpleScene.h>
#include "LabCamera.h"

class Tema2 : public SimpleScene
{
	public:

		Tema2();
		~Tema2();

		void Init() override;

		struct ViewportSpace
		{
			ViewportSpace() : x(0), y(0), width(1), height(1) {}
			ViewportSpace(int x, int y, int width, int height)
				: x(x), y(y), width(width), height(height) {}
			int x;
			int y;
			int width;
			int height;
		};

		ViewportSpace viewSpaceMiniMap, viewSpaceWeaponsStats;
		void SetViewportArea(const ViewportSpace & viewSpace, glm::vec3 colorColor, bool clear);
		

	private:
		void FrameStart() override;
		void deleteOutOfRangeBullets();
		void deleteUsedGrenades();
		void generateEnemies();
		void drawEnemies();
		void steveFallAnimation();
		void computeCollisions();
		void deleteDeadEnemies();
		void towerDefense();
		glm::mat4 myRotateOY(float cos, float sin);
		void drawTowers();
		void drawSteve();
		void drawLifes();
		void drawBullets();
		void drawGrenades();
		void drawBeginGate();
		void drawEndGate();
		void drawScene();
		void drawWeaponsStats();
		void Update(float deltaTimeSeconds) override;
		void FrameEnd() override;

		void RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix) override;

		void MyRenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix, const glm::mat4 & viewMatrix);

		void OnInputUpdate(float deltaTime, int mods) override;
		void OnKeyPress(int key, int mods) override;
		void OnKeyRelease(int key, int mods) override;
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
		void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
		void OnWindowResize(int width, int height) override;

	protected:
		Laborator::Camera *camera, *mapCamera, *cameraTP, *cameraFP;
		glm::mat4 projectionMatrix;
		bool renderCameraTarget;
};
