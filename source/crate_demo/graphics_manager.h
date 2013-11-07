/* The Halfling Project - A Graphics Engine and Projects
 *
 * The Halfling Project is the legal property of Adrian Astley
 * Copyright Adrian Astley 2013
 */

#ifndef CRATE_DEMO_GRAPHICS_MANAGER_H
#define CRATE_DEMO_GRAPHICS_MANAGER_H

#include "common/graphics_manager_base.h"

#include <d3d11.h>

namespace CrateDemo {

class GameStateManager;
class GraphicsManager : public Common::GraphicsManagerBase {
public:
	GraphicsManager(GameStateManager *gameStateManager);

private:
	GameStateManager *m_gameStateManager;

	ID3D11RenderTargetView *m_renderTargetView;




public:
	bool Initialize(int clientWidth, int clientHeight, HWND hwnd);
	void Shutdown();
	void DrawFrame();
	void OnResize(int newClientWidth, int newClientHeight);
	void GamePaused();
	void GameUnpaused();
};

} // End of namespace CrateDemo

#endif
