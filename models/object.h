
/*
 * Class Object: Polygon and PointRG
 * ...........................................................................
 */

class Object
{
protected:
    Rectangle _MBR;
    std::string identifier;
public:
    Rectangle* getMBR(){return &this->_MBR;};
    virtual void updateMBR() = 0;
    virtual bool iswithin(Object    &d) = 0;
    virtual bool iswithin(Rectangle *d) = 0;
    virtual DataType howMuchGrow(Rectangle *r) = 0;
    virtual void* search(Object *dat) = 0;
};