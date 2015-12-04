#ifndef __LIBSOCKETIO_ROVCONTROL__
#define __LIBSOCKETIO_ROVCONTROL__
#include "SocketIO.h"

typedef float f32;

const f32 ROUNDING_ERROR_f32 = 0.000001f;

inline bool equals(const f32 a, const f32 b, const f32 tolerance = ROUNDING_ERROR_f32)
{
	return (a + tolerance >= b) && (a - tolerance <= b);
}

inline std::string& operator + (std::string& s,float f) {
    char tmp[32]{ 0 };
	sprintf(tmp, "%.2f", f);
    s += tmp;

    return s;
}
class RovControl
    :public SocketIO::SIODelegate {
public:
    const float	FORWARD_VALUE = -1.0f;
    const float BACK_VALUE = 1.0f;
    const float LEFT_VALUE = -1.0f;
    const float RIGHT_VALUE = 1.0f;
    const float UP_VALUE = 1.0f;
    const float DOWN_VALUE = -1.0f;

    class Position {
	public:
		Position():throttle(0.f),yaw(0.f),lift(0.f){}

		bool equals(const Position& p) const {
			return (::equals(p.lift,lift)&&::equals(p.throttle,throttle)&&::equals(p.yaw,yaw));
		}

		void assign(const Position& p) {
			this->lift = p.lift;
			this->throttle = p.throttle;
			this->yaw = p.yaw;
		}

		std::string toJSONString() const{
			std::string tmp = "{\"throttle\":";
            
            tmp = tmp + throttle;
			tmp.append(",\"yaw\":");
            tmp = tmp + yaw;
			tmp.append(",\"lift\":");
            tmp = tmp + lift;
			tmp.append("}");

			return tmp;
		}

	public:
		float throttle;
		float yaw;
		float lift;
	};
public:
    RovControl();
    virtual ~RovControl();

    void onClose(SIOClient* client);

    virtual void onError(SIOClient* client, const std::string& data);
};
#endif
