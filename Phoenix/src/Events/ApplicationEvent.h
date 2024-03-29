#pragma once

#include "Event.h"

namespace Phoenix
{
  class WindowCloseEvent : public Event
  {
  public:
    WindowCloseEvent() {}

    inline static EventType getEventType() { return EventType::WindowClose; }

    virtual EventType getType() const override { return getEventType(); }
    virtual int getCategory() const override { return EventCategory::EventApplication; }
    virtual const char* getName() const override { return "Event::WindowClose"; }
  };

  class WindowResizeEvent : public Event
  {
  public:
    WindowResizeEvent(unsigned int _width, unsigned int _height) : width(_width), height(_height) {}

    inline static EventType getEventType() { return EventType::WindowResize; }

    virtual EventType getType() const override { return getEventType(); }
    virtual int getCategory() const override { return EventCategory::EventApplication; }
    virtual const char* getName() const override { return "Event::WindowResize"; }

    std::string toString() const override
    {
      std::stringstream ss;
      ss << getName() << " - " << width << "x" << height;
      return ss.str();
    }

  private:
    unsigned int width, height;
  };

  class WindowFocusEvent : public Event
  {
  public:
    WindowFocusEvent() {}

    inline static EventType getEventType() { return EventType::WindowFocus; }

    virtual EventType getType() const override { return getEventType(); }
    virtual int getCategory() const override { return EventCategory::EventApplication; }
    virtual const char* getName() const override { return "Event::WindowFocus"; }
  };

  class WindowUnfocusEvent : public Event
  {
  public:
    WindowUnfocusEvent() {}

    inline static EventType getEventType() { return EventType::WindowUnfocus; }

    virtual EventType getType() const override { return getEventType(); }
    virtual int getCategory() const override { return EventCategory::EventApplication; }
    virtual const char* getName() const override { return "Event::WindowUnfocus"; }
  };
}
