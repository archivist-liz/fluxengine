#include "globals.h"
#include "ui.h"
#include "uipp.h"
#include "gui.h"
#include "threads.h"

using namespace std::placeholders;

static uiDrawStrokeParams STROKE = {
	uiDrawLineCapFlat,
	uiDrawLineJoinMiter,
	0.5,
	1.0,
	nullptr,
	0,
	0.0
};

static uiDrawBrush WHITE = { uiDrawBrushTypeSolid, 1.0, 0.0, 1.0, 1.0 };
static uiDrawBrush BLACK = { uiDrawBrushTypeSolid, 0.0, 0.0, 0.0, 1.0 };


class MainApp : public UIAllocator, public Showable
{
public:
	MainApp()
	{
		_window = make<UIWindow>("FluxEngine", 640, 480)
			->setOnClose(std::bind(&MainApp::onClose, this))
			->setChild(
				make<UIVBox>()
					->add(make<UIHBox>()
						->add(_busyButton = make<UIButton>("busy button"))
						->add(make<UIButton>("press me!"))
					)
					->add(make<UIArea>()
						->setOnDraw(std::bind(&MainApp::redrawArea, this, _1))
						->setStretchy(true))
					->add(make<UIButton>("press me again!"))
			);

		UIStartAppThread(
			std::bind(&MainApp::appThread, this),
			std::bind(&MainApp::appThreadExited, this));
	}

	void show()
	{
		_window->show();
	}

	void hide()
	{
		_window->hide();
	}

private:
	int onClose()
	{
		uiQuit();
		return 1;
	}

	void redrawArea(uiAreaDrawParams* p)
	{
		UIPath(p).rectangle(0, 0, p->AreaWidth, p->AreaHeight).fill(WHITE);
		UIPath(p).begin(0, 0).lineTo(p->AreaWidth, p->AreaHeight).end().stroke(BLACK, STROKE);
	}

	void appThread()
	{
		UIRunOnUIThread([&] { _busyButton->setText("Busy"); });
		sleep(5);
	}

	void appThreadExited()
	{
		_busyButton->setText("Not busy");
	}

private:
	UIWindow* _window;
	UIButton* _busyButton;
};

std::unique_ptr<Showable> createMainApp()
{
	return std::make_unique<MainApp>();
}

