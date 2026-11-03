#include <D3DFrame/D3DApp.h>

class D3DFrame: public D3DApp {
public:
    D3DFrame(HINSTANCE hInstance);
    ~D3DFrame();

public:
    virtual bool Initialize();
    virtual void Update(const GameTimer&);
    virtual void Render(const GameTimer&);
    
};