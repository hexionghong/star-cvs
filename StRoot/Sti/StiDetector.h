/*
 * StiDetector represents a detector for the purposes of ITTF tracking.
 * It contains all information about the geometry of the detector and
 * the necessary physical properties for incorporating it in tracking.
 */

#ifndef STI_DETECTOR_HH
#define STI_DETECTOR_HH

#include <string>

class StiMaterial;
class StiShape;
class StiPlacement;

class StiDetector {
    
public:

    // con/destructor
    StiDetector();
    virtual ~StiDetector();
    
    // accessors
    bool isOn() const {return on;}
    bool isActive() const { return active; }
    bool isContinuousMedium() const { return continuousMedium; }
    bool isDiscreteScatterer() const { return discreteScatterer; }

    StiMaterial* getGas() const { return gas; }
    StiMaterial* getMaterial() const { return material; }

    StiShape* getShape() const { return shape; }
    StiPlacement* getPlacement() const { return placement; }

    const char* getName() const {return name;}
    
    // mutators
    void setIsOn(bool val) {on = val;}
    void setIsActive(bool val) {active = val;}
    void setIsContinuousMedium(bool val) {continuousMedium = val;}
    void setIsDiscreteScatterer(bool val) {discreteScatterer = val;}

    void setGas(StiMaterial *val){ gas = val; }
    void setMaterial(StiMaterial *val){ material = val; }

    void setShape(StiShape *val){ shape = val; }
    void setPlacement(StiPlacement *val){ placement = val; }

    void setName(const char *val){	strncpy(name, val, 99);}

    //action
    virtual void build(const char* infile);  //for now, build from SCL parsable ascii file
    virtual void write(const char* szFileName);

    virtual void copy(StiDetector &detector);
    
protected:
    
    // logical switches
    bool on;                  // toggle this layer on/off.  (off => NOT added to detector container)
    bool active;              // does the object provide hit information?
    bool continuousMedium;    // is this a continuous scatterer?  (yes => scatterer info given by "gas" below)
    bool discreteScatterer;   // is this a discrete scatterer?    (yes => scatterer given by "material" below)

    // material information
    StiMaterial *gas;           // gas representing the atmosphere in 
    //   (if it's a continuous medium) and/or  radially inward from the detector.
    StiMaterial *material;      // material composing the discrete scatterer
    
    // physical location / orientation
    StiShape     *shape;
    StiPlacement *placement;

    char name[100];  //Name of the class, a char to avoid template problems

};

//Non-members--------------------------
//ostream& operator<<(ostream&, const StiDetector&);

#endif
