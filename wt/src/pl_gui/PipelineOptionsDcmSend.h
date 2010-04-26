//
//
//  Description:
//      This is the widget for DICOM Send pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONSDCMSEND_H
#define PIPELINEOPTIONSDCMSEND_H

#include "PipelineOptions.h"

namespace Wt
{
    class WLineEdit;
    class WComboBox;
};

///
//  Constants
//
const int FIRST_OPTIONAL_DCMSEND_STAGE = 1; // No required stages
const int NUM_OPTIONAL_DCMSEND_STAGES = 1;

///
/// \class PipelineOptionsDcmSend
/// \brief Class for options to configure DICOM Send pipeline
///
class PipelineOptionsDcmSend : public PipelineOptions
{
public:
    ///
    /// Constructor
    ///
    PipelineOptionsDcmSend(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptionsDcmSend();

    ///
    /// Reset to default state
    ///
    void resetAll();

    ///
    /// Check whether the options are valid
    ///
    virtual bool validate() const;

    ///
    /// Generate command-line options string based on user choices
    ///
    virtual std::string getCommandLineString() const;

private:

    ///
    ///  Anonymize checkbox clicked [slot]
    ///
    void anonymizeClicked();



private:

    /// Buttons for pipeline stages
    WCheckBox *mStageBoxes[NUM_OPTIONAL_DCMSEND_STAGES];

    /// Hostname
    WLineEdit *mHostNameEdit;

    /// Port
    WLineEdit *mPortEdit;

    /// AE Title
    WLineEdit *mAETitleEdit;

    /// Anonymize
    WCheckBox *mAnonymizeCheckBox;

    /// Anonymization type combo box
    WComboBox *mAnonymizeComboBox;

};

#endif // PIPELINEOPTIONSFS_H

