#include <string>


class NotifyMessage {
private:
    std::string text;
    float posx, posy;
    float duration, startTime;


public:
    // Getter methods
    std::string getText() const { return text; }
    float get_posx() const { return posx; }
    float get_posy() const { return posy; }
    float getDuration() const { return duration; }

    // Method to check if the message is still active
    bool isActive(float currentTime) const {
        return (currentTime - startTime) < duration;
    }
};