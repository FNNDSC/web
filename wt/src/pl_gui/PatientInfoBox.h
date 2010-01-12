//
//
//  Description:
//      Definition of patient info box widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PATIENTINFOBOX_H
#define PATIENTINFOBOX_H

#include <Wt/WContainerWidget>
#include <Wt/WGroupBox>
#include <vector>
#include <string>
#include "GlobalEnums.h"

namespace Wt
{
    class WLabel;
}

using namespace Wt;

///
/// \class PatientInfoBox
/// \brief Provides a group box that displays patient info from "toc.txt" file
///
class PatientInfoBox : public WGroupBox
{
public:

    ///
    /// Constructor
    ///
    PatientInfoBox(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PatientInfoBox();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// \brief Set the scan directory
    ///
    void setScanDir(std::string scanDir);

private:

    ///
    ///	Set the patient information text, do not display "ERROR:" if that is what is present
    ///
    void setInfoLabel(WLabel* label, const std::string &str);

private:

    /// Patient ID
    WLabel *mPatientID;

    /// Patient Name
    WLabel *mPatientName;

    /// Patient age
    WLabel *mPatientAge;

    /// Patient Sex
    WLabel *mPatientSex;

    /// Patient birthday
    WLabel *mPatientBirthday;

    /// Scan date
    WLabel *mImageScanDate;

    /// Scanner manufacturer
    WLabel *mScannerManufacturer;

    /// Scanner model
    WLabel *mScannerModel;

    /// Software version
    WLabel *mSoftwareVer;
};

#endif // SCANBROWSER_H

