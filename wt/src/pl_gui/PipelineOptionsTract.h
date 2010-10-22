//
//
//  Description:
//      This is the widget for Tractography pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONSTRACT_H
#define PIPELINEOPTIONSTRACT_H

#include "PipelineOptions.h"
#include <string>

///
//  Constants
//
const int FIRST_OPTIONAL_TRACT_STAGE = 3; // Stages 1 and 2 are required
#if 0 // Temporarily disable stage 4 and 5
    const int NUM_OPTIONAL_TRACT_STAGES = 3;
#else
    const int NUM_OPTIONAL_TRACT_STAGES = 1;
#endif

namespace Wt
{
    class WCheckBox;
    class WComboBox;
    class WLineEdit;
    class WFileUpload;
    class WMessageBox;
};

///
/// \class PipelineOptionsTract
/// \brief Class for options to configure tractography pipeline
///
class PipelineOptionsTract : public PipelineOptions
{
public:
    ///
    /// Constructor
    ///
    PipelineOptionsTract(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptionsTract();

    ///
    ///  Reset to default state
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
    ///  Volume mask 2 checkbox clicked [slot]
    ///
    void volumeMask2Clicked();

    ///
    ///  B0 Volumes checkbox clicked [slot]
    ///
    void b0VolumeClicked();

    ///
    ///  Gradient file uploaded [slot]
    ///
    void fileUploaded();

    ///
    /// Handle message box finished [slot]
    ///
    void handleMessageBoxFinished(StandardButton dialogCode);

protected:

    /// Buttons for pipeline stages
    WCheckBox *mStageBoxes[NUM_OPTIONAL_TRACT_STAGES];

    /// Checkbox for performing Eddy Current correction
    WCheckBox *mEddyCurrentCheckBox;

    /// Reconstruction algorithm
    WComboBox *mReconAlgorithmComboBox;

    /// Image model
    WComboBox *mImageModelComboBox;

    /// Mask volume combo box
    WComboBox *mVolumeMask1ComboBox;

    /// Volume mask threshold
    WLineEdit *mLowerThreshold1LineEdit;

    /// Mask Lower Threshold label
    WLabel *mLowerThreshold1Label;

    /// Volume mask threshold
    WLineEdit *mUpperThreshold1LineEdit;

    /// Mask Upper Threshold label
    WLabel *mUpperThreshold1Label;

    /// Checkbox for whether to use FA volume as mask filter
    WCheckBox *mVolumeMask2CheckBox;

    /// Mask volume combo box
    WComboBox *mVolumeMask2ComboBox;

    /// Volume mask threshold
    WLineEdit *mLowerThreshold2LineEdit;

    /// Mask Lower Threshold label
    WLabel *mLowerThreshold2Label;

    /// Volume mask threshold
    WLineEdit *mUpperThreshold2LineEdit;

    /// Mask Upper Threshold label
    WLabel *mUpperThreshold2Label;

    /// Upload Gradient file check box
    WCheckBox *mGradientFileCheckBox;

    /// Gradient Table File upload
    WFileUpload *mGradientFileUpload;

    /// Name of uploaded gradient file on server
    std::string mGradientServerFile;

    /// Gradient invert X
    WCheckBox *mGradientInvertX;

    /// Gradient invert Y
    WCheckBox *mGradientInvertY;

    /// Gradient invert X
    WCheckBox *mGradientInvertZ;

    /// Manually set B0 volumes check box
    WCheckBox *mB0VolumesCheckBox;

    /// Number of B0 volumes label
    WLabel *mB0VolumesLabel;

    /// Number of B0 Volumes
    WLineEdit *mB0VolumesLineEdit;

};

#endif // PIPELINEOPTIONSTRACT_H

