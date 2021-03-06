#pragma once
#include "MessageStream.hpp"
class Message;

#include "Shared/AbstractException.hpp"
DefineNewException(UnsupportedMessageException)

template <class... Ts> struct overloaded : Ts... { overloaded(Ts... f) : Ts(f)... {} };
template <class... Ts> auto overload(Ts... f) { return overloaded<Ts...>(f...); }

template <typename lambda> 
void parse_message_from_server(Message const& message, lambda action, std::shared_ptr<Objects> objects) {
	ObjectsStatic::initialize(objects);

	MessageInputStream s(message);
	MessageType type;
	s >> type;
	switch (type) {
		case MessageType::Connection: {
			uint8_t index;
			s >> index;
			action(index);
			break;
		}
		case MessageType::Map: {
			std::shared_ptr<Map> map;
			s >> map;
			action(map);
			break;
		}
		case MessageType::ActorQueue: {
			MainActorQueue queue;
			s >> queue;
			action(queue);
			break;
		}
		case MessageType::ActorQueueUpdate: {
			MainActorQueue queue;
			s >> action(Update<MainActorQueue>(queue));
			break;
		}
		case MessageType::ProjectileQueueUpdate: {
			ProjectileQueue queue;
			s >> queue;
			action(queue);
			break;
		}
		default:
			throw Exceptions::UnsupportedMessageException("Unsupported message was passed for parsing.");
	}
};
template <typename lambda>
void parse_message_from_client(Message const& message, lambda action) {
	MessageInputStream s(message);
	MessageType type;
	s >> type;
	switch (type) {
		case MessageType::ControlEvent: {
			ControlEvent ev;
			bool dir;
			s >> ev >> dir;
			action(ev, dir);
			break;
		}
		case MessageType::AimEvent: {
			float x, y;
			s >> x >> y;
			action(x, y);
			break;
		}
		default:
			throw Exceptions::UnsupportedMessageException("Unsupported message was passed for parsing.");
	}
};