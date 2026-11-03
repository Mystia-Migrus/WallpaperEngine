#include <D3DFrame.h>

D3DFrame::D3DFrame(HINSTANCE hInstance): D3DApp(hInstance) {

}

D3DFrame::~D3DFrame() {

}

bool D3DFrame::Initialize() {
    if(!D3DApp::Initialize())
        return false;
    
    return true;
}

void D3DFrame::Update(const GameTimer& timer) {

}

void D3DFrame::Render(const GameTimer& timer) {

}