#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

int main() {
    int fd = open("/dev/input/js1", O_RDONLY);
    if (fd == -1) {
        perror("Could not open joystick");
        return 1;
    }

    char name[128];
    if (ioctl(fd, JSIOCGNAME(sizeof(name)), name) < 0) {
        perror("Error getting device name");
        close(fd);
        return 1;
    }

    printf("Joystick Name: %s\n", name);  // This may show something like "Xbox
                                          // 360 Wired Controller"


    struct js_event e;
    while (1) {
        if (read(fd, &e, sizeof(struct js_event)) == -1) {
            perror("Error reading joystick");
            return 1;
        }
        if( 0 )printf( "Read done, %d\n", e.type );
        
        switch (e.type) {
            case JS_EVENT_BUTTON:
                printf("Button %d %s\n", e.number, e.value ? "pressed" : "released");
                break;
            case JS_EVENT_AXIS:
                if( e.value > 4000 || e.value < -4000 )
                    printf("Axis %d value: %d\n", e.number, e.value);
                if( e.number == 6 ||
                    e.number == 7 ) {
                    printf("Axis %d value: %d\n", e.number, e.value);
                    }
                break;
            default:
                break;
        }
    }
    
    close(fd);
    return 0;
}
