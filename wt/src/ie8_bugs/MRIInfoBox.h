//
//
//  Description:
//      Definition of MRI info box widget
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef MRIINFOBOX_H
#define MRIINFOBOX_H

#include <Wt/WContainerWidget>
#include <Wt/WGroupBox>
#include <vector>
#include <string>
//#include "GlobalEnums.h"

namespace Wt
{
    class WLabel;
    class WTextArea;
}

using namespace Wt;

///
/// \class MRIInfoBox
/// \brief Provides a group box that displays MRI Info from mri_info_batch.bash generated files.
///
class MRIInfoBox : public WGroupBox
{
public:

    ///
    /// Constructor
    ///
    MRIInfoBox(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~MRIInfoBox();

    ///
    /// Reset all widgets to the default state
    ///
    void resetAll();

    ///
    /// \brief Set the DICOM file name for which to find info
    ///
    void setDicomFileName(std::string dcmFileName);

private:

private:

    /// MRI Info
    WTextArea *mMRIInfoTextArea;
};

#endif // MRIINFOBOX_H

