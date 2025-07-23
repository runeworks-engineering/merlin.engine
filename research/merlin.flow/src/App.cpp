#include "merlin.h"
#include "Mainlayer.h"

using namespace Merlin;

class App : public Application
{
public:
	App()
		: Application("OpenGL Examples")
	{
		pushLayer(new MainLayer());
	}
};

int main()
{
	std::unique_ptr<App> app = std::make_unique<App>();
	app->run();
}