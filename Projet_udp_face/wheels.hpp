#ifndef WHEELS_HPP
#define WHEELS_HPP

#include <libusb-1.0/libusb.h>

class Wheels
{
    private:
        bool deviceMatchesVendorProduct(libusb_device *device, unsigned short idVendor, unsigned short idProduct);

    public:
        // La seule methode bas-niveau utilise elle fixe directement la position du servomoteur
        // dans notre cas (servo-moteur a rotation continue) elle gere la vitesse de rotation
        void setTarget(int position, int servo);

};

#endif // WHEELS_HPP
