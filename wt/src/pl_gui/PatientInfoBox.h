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
    class WTreeView;
    class WStandardItemModel;
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
    typedef enum
    {
        PATIENT_ID = 0,
        PATIENT_NAME = 1,
        PATIENT_AGE = 2,
        PATIENT_SEX = 3,
        PATIENT_BIRTHDAY = 4,
        IMAGE_SCAN_DATE = 5,
        SCANNER_MANUFACTURER = 6,
        SCANNER_MODEL = 7,
        SOFTWARE_VERSION = 8,
        NUM_ROWS
    } PatientEnum;

    ///
    ///	Set the patient information text, do not display "ERROR:" if that is what is present
    ///
    void setInfoData(PatientEnum patientEnum, const std::string& str);

private:


    /// Tree view of patient info
    WTreeView *mTreeView;

    /// Model data
    WStandardItemModel *mModel;
};

#endif // SCANBROWSER_H

