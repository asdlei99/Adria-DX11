#pragma once
#include <array>
#include <unordered_map>
#include "Core/CoreTypes.h"
#include "Core/Windows.h"
#include "Events/Delegate.h"
#include "Utilities/Singleton.h"

namespace adria
{
    enum class KeyCode : uint32
    {
       
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
        Q, W, E, R, T, Y, U, I, O, P,
        A, S, D, F, G, H, J, K, L,
        Z, X, C, V, B, N, M,
        Esc,
        Tab,
        ShiftLeft, ShiftRight,
        CtrlLeft, CtrlRight,
        AltLeft, AltRight,
        Space,
        CapsLock,
        Backspace,
        Enter,
        Delete,
        ArrowLeft, ArrowRight, ArrowUp, ArrowDown,
        PageUp, PageDown,
        Home,
        End,
        Insert,
        MouseLeft,
        MouseMiddle,
        MouseRight
    };

    struct WindowMessage;

    DECLARE_EVENT(WindowResizedEvent, Input, uint32, uint32);
    DECLARE_EVENT(LeftMouseClickedEvent, Input, int32, int32);
	DECLARE_EVENT(MiddleMouseScrolledEvent, Input, int32);
	DECLARE_EVENT(F5PressedEvent, Input);

    struct InputEvents
    {
        MiddleMouseScrolledEvent scroll_mouse_event;
		LeftMouseClickedEvent left_mouse_clicked_event;
		WindowResizedEvent window_resized_event;
        F5PressedEvent f5_pressed_event;
    };

    class Input : public Singleton<Input>
    {
        friend class Singleton<Input>;
    public:
        void NewFrame();
        void HandleWindowMessage(WindowMessage const&);

        bool GetKey(KeyCode key)    /*const*/   { return keys[key]; }                         
        bool IsKeyDown(KeyCode key) /*const*/   { return GetKey(key) && !prev_keys[key]; }
        bool IsKeyUp(KeyCode key)   /*const*/   { return !GetKey(key) && prev_keys[key]; }

        // Mouse
        void SetMouseVisible(bool visible);
        void SetMousePosition(float xpos, float ypos);

        float GetMousePositionX()  const { return mouse_position_x; }
        float GetMousePositionY()  const { return mouse_position_y; }

        float GetMouseDeltaX()     const { return mouse_position_x - prev_mouse_position_x;/*return mouse_delta_x;*/ }
        float GetMouseDeltaY()     const { return mouse_position_y - prev_mouse_position_y;/*return mouse_delta_y;*/ }
        float GetMouseWheelDelta() const { return m_mouse_wheel_delta; }

		InputEvents& GetInputEvents() { return input_events; }

    private:
        InputEvents input_events;
        std::unordered_map<KeyCode, bool> keys;
        std::unordered_map<KeyCode, bool> prev_keys;
        // Mouse
        float mouse_position_x = 0.0f;
        float mouse_position_y = 0.0f;

        float prev_mouse_position_x = 0.0f;
        float prev_mouse_position_y = 0.0f;
        float m_mouse_wheel_delta = 0.0f;

        bool new_frame = false;
        bool resizing = false;

    private:
		Input();
    };
    #define g_Input Input::Get()
}