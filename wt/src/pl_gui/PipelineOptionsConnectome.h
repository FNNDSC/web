//
//
//  Description:
//      This is the widget for Connectome pipeline options
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//
#ifndef PIPELINEOPTIONSCONNECTOME_H
#define PIPELINEOPTIONSCONNECTOME_H

#include "PipelineOptionsTract.h"
#include <string>

///
//  Constants
//

namespace Wt
{
    class WCheckBox;
    class WComboBox;
    class WLineEdit;
    class WFileUpload;
    class WMessageBox;
};

///
/// \class PipelineOptionsConnectome
/// \brief Class for options to configure connectome pipeline
///
class PipelineOptionsConnectome : public PipelineOptionsTract
{
public:
    ///
    /// Constructor
    ///
    PipelineOptionsConnectome(WContainerWidget *parent = 0);

    ///
    /// Destructor
    ///
    virtual ~PipelineOptionsConnectome();

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

    /// Taliarach Freesurfer option
    WCheckBox *mTalairachCheckBox;

};

#endif // PIPELINEOPTIONSTRACT_H

