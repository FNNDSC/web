//
//
//  Description:
//      Global enumerants needed throughout the project
//
//  Author:
//      Dan Ginsburg
//
//  Children's Hospital Boston
//  GPL v2
//

#ifndef GLOBALENUMS_H
#define GLOBALENUMS_H

namespace Enums
{
    typedef enum
    {
        PIPELINE_UNKNOWN = -1,
        PIPELINE_TYPE_TRACT,
        PIPELINE_TYPE_FS,
        PIPELINE_TYPE_FETAL,
        PIPELINE_TYPE_DCMSEND,

        NUM_PIPELINES

    } PipelineType;
}

#endif // GLOBALENUMS_H
