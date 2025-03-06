//
// Created by Jay Vaughan on 04.03.25.
//

#ifndef APCSEQUENCER_LOAD81COMPONENT_H
#define APCSEQUENCER_LOAD81COMPONENT#include <juce_gui_basics/juce_gui_basics.h>
#include <lua.hpp>
#include <memory>
#include <unordered_map>

// Lua bindings for JUCE graphics
struct LuaGraphicsContext {
	juce::Graphics* g;  // Pointer to JUCE Graphics context for rendering
};

// Simple key state tracking
struct KeyboardState {
	std::unordered_map<int, bool> pressed;
};

// Main LOAD81-inspired component
class Load81Component : public juce::Component, public juce::Button::Listener, public juce::KeyListener {
public:
	Load81Component() {
		// Initialize Lua state
		L = luaL_newstate();
		luaL_openlibs(L);

		// Set up UI components
		editor.setMultiLine(true);
		editor.setReturnKeyStartsNewLine(true);
		addAndMakeVisible(editor);

		runButton.setButtonText("Run");
		runButton.addListener(this);
		addAndMakeVisible(runButton);

		// Initially show editor
		isRuntimeActive = false;
		setSize(800, 600);

		// Register this as a key listener
		addKeyListener(this);
		setWantsKeyboardFocus(true);
	}

	~Load81Component() override {
		lua_close(L);
	}

	void paint(juce::Graphics& g) override {
		g.fillAll(juce::Colours::black);  // Retro black background
		if (isRuntimeActive) {
			// Pass Graphics context to Lua
			LuaGraphicsContext ctx{&g};
			lua_pushlightuserdata(L, &ctx);
			lua_setglobal(L, "g");

			// Call Lua's draw function
			lua_getglobal(L, "draw");
			if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
				juce::String error = lua_tostring(L, -1);
				juce::Logger::outputDebugString("Lua error: " + error);
				lua_pop(L, 1);
			}
		}
	}

	void resized() override {
		auto bounds = getLocalBounds();
		if (!isRuntimeActive) {
			editor.setBounds(bounds.reduced(10));
			runButton.setBounds(bounds.removeFromBottom(40).reduced(10, 5));
		}
	}

	void buttonClicked(juce::Button* button) override {
		if (button == &runButton) {
			executeLuaCode();
			isRuntimeActive = true;
			editor.setVisible(false);
			runButton.setVisible(false);
			repaint();
		}
	}

	bool keyPressed(const juce::KeyPress& key, juce::Component*) override {
		// ESC to toggle back to editor
		if (key == juce::KeyPress::escapeKey && isRuntimeActive) {
			isRuntimeActive = false;
			editor.setVisible(true);
			runButton.setVisible(true);
			repaint();
			return true;
		}
		// Update keyboard state for Lua
		keyboard.pressed[key.getKeyCode()] = true;
		return false;
	}

	bool keyStateChanged(bool isKeyDown, juce::Component*) override {
		// Track key releases
		if (!isKeyDown) {
			auto key = juce::KeyPress::getCurrentlyDownKeyCode();
			keyboard.pressed[key] = false;
		}
		return false;
	}

private:
	lua_State* L;
	juce::TextEditor editor;
	juce::TextButton runButton;
	bool isRuntimeActive;
	KeyboardState keyboard;

	void registerLuaBindings() {
		// Bind fill function
		lua_pushcfunction(L, [](lua_State* L) -> int {
			auto* ctx = static_cast<LuaGraphicsContext*>(lua_touserdata(L, lua_upvalueindex(1)));
			float r = luaL_checknumber(L, 1);
			float g = luaL_checknumber(L, 2);
			float b = luaL_checknumber(L, 3);
			ctx->g->fillAll(juce::Colour::fromFloatRGBA(r, g, b, 1.0f));
			return 0;
		});
		lua_setglobal(L, "fill");

		// Bind rect function
		lua_pushcfunction(L, [](lua_State* L) -> int {
			auto* ctx = static_cast<LuaGraphicsContext*>(lua_touserdata(L, lua_upvalueindex(1)));
			float x = luaL_checknumber(L, 1);
			float y = luaL_checknumber(L, 2);
			float w = luaL_checknumber(L, 3);
			float h = luaL_checknumber(L, 4);
			ctx->g->drawRect(x, y, w, h);
			return 0;
		});
		lua_setglobal(L, "rect");

		// Bind keyboard.pressed table
		lua_newtable(L);
		lua_pushvalue(L, -1);  // Keep a reference for updates
		lua_setglobal(L, "keyboard");
	}

	void executeLuaCode() {
		// Register bindings before running code
		registerLuaBindings();

		// Load and execute Lua code from editor
		juce::String code = editor.getText();
		if (luaL_dostring(L, code.toRawUTF8()) != LUA_OK) {
			juce::String error = lua_tostring(L, -1);
			juce::Logger::outputDebugString("Lua execution error: " + error);
			lua_pop(L, 1);
		}
	}
};

#ifdef TEST_LOAD81_APP
// JUCE Application
class Load81App : public juce::JUCEApplication {
public:
	Load81App() {}

	const juce::String getApplicationName() override { return "Load81 JUCE"; }
	const juce::String getApplicationVersion() override { return "1.0"; }
	bool moreThanOneInstanceAllowed() override { return true; }

	void initialise(const juce::String&) override {
		mainWindow = std::make_unique<juce::DocumentWindow>(
				getApplicationName(),
				juce::Colours::black,
				juce::DocumentWindow::allButtons);
		mainWindow->setContentOwned(new Load81Component(), true);
		mainWindow->centreWithSize(800, 600);
		mainWindow->setVisible(true);
	}

	void shutdown() override {
		mainWindow.reset();
	}

private:
	std::unique_ptr<juce::DocumentWindow> mainWindow;
};

// JUCE entry point
START_JUCE_APPLICATION(Load81App)
#endif

#endif //APCSEQUENCER_LOAD81COMPONENT_H
