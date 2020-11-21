/*
 * mmvtkmStreamLines.h
 *
 * Copyright (C) 2020 by VISUS (Universitaet Stuttgart)
 * Alle Rechte vorbehalten.
 */

#pragma once

#include "mmcore/CalleeSlot.h"
#include "mmcore/CallerSlot.h"
#include "mmcore/Module.h"
#include "mmcore/param/ParamSlot.h"

#include "mesh/MeshCalls.h"


typedef vislib::math::Point<float, 3> visPoint3f;
typedef vislib::math::Vector<float, 3> visVec3f;
typedef vislib::math::Plane<float> visPlanef;


namespace megamol {
namespace mmvtkm {


class mmvtkmStreamLines : public core::Module {
public:
    /**
     * Answer the name of this module.
     *
     * @return The name of this module.
     */
    static const char* ClassName() { return "vtkmStreamLines"; }

    /**
     * Answer a human readable description of this module.
     *
     * @return A human readable description of this module.
     */
    static const char* Description() {
        return "Creates streamlines for a vtkm tetraeder mesh and converts the streamlines "
               "into megamols CallMesh";
    }

    /**
     * Answers whether this module is available on the current system.
     *
     * @return 'true' if the module is available, 'false' otherwise.
     */
    static bool IsAvailable() { return true; }

    /**
     * Ctor
     */
    mmvtkmStreamLines();

    /**
     * Dtor
     */
    virtual ~mmvtkmStreamLines();

protected:
    /**
     * Implementation of 'Create'.
     *
     * @return 'true' on success, 'false' otherwise.
     */
    bool create();

    /**
     * Implementation of 'Release'.
     */
    void release();

    /**
     * Gets the data from the source.
     *
     * @param caller The calling call.
     *
     * @return 'true' on success, 'false' on failure.
     */
    bool getDataCallback(core::Call& caller);

    /**
     * Gets the meta data from the source.
     *
     * @param caller The calling call.
     *
     * @return 'true' on success, 'false' on failure.
     */
    bool getMetaDataCallback(core::Call& caller);


private:
    enum planeMode { NORMAL, PARAMETER };

    struct Triangle {
        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;
        glm::vec3 o;

        glm::vec3 v1;
        glm::vec3 v2;

        float area = 0.f;

        // used for sampling within surrounding polygon
        // weight = triangle_area / polygon_area
        float weight = 0.f;

        Triangle(const glm::vec3& rhsA, const glm::vec3& rhsB, const glm::vec3& rhsC) : a(rhsA), b(rhsB), c(rhsC) {
            float ab = glm::length(rhsB - rhsA);
            float ac = glm::length(rhsC - rhsA);
            float bc = glm::length(rhsC - rhsB);
            float max = std::max(ab, std::max(ac, bc));

            // get hypothenuse in v1 and adjacent in v2
            if (ab == max) {
                this->v1 = rhsB - rhsA;
                this->v2 = rhsC - rhsA;
                this->o = rhsA;
            } else if (ac == max) {
                this->v1 = rhsC - rhsA;
                this->v2 = rhsB - rhsA;
                this->o = rhsA;
            } else {
                this->v1 = rhsC - rhsB;
                this->v2 = rhsA - rhsB;
                this->o = rhsB;
            }

            float v1_length = glm::length(v1);
            float v2_length = glm::length(v2);

            // alternatively:
            // float cos_theta = v1.Dot(v2) / (v1_length * v2_length);
            // float sin_theta = sqrt(1.f - cos_theta * cos_theta)
            float arg = glm::dot(v1, v2) / (v1_length * v2_length); // angle via dot product
            float theta = acos(trunc(arg * 1000000.f) / 1000000.f); // avoids -nan in late decimal positions
            this->area = 0.5f * v1_length * v2_length * sin(theta);
        }
    };

	/** Callback function to rotate the ghostplane around its normal */
	bool rotateGhostPlane(core::param::ParamSlot& slot);

	/** Callback function to rotate the seedplane around its normal */
	bool rotateSeedPlane(core::param::ParamSlot& slot);

	/** Callback function to assign altered u and v values */
    bool assignSTPQ(core::param::ParamSlot& slot);

	/** 
	* Calculates the liveSeedPlane_ with the current u and v values 
	* 
	* @param isU, true for u, false for v
	*/
    void calcLiveSeedPlane(
        unsigned int line, float uv, const glm::vec3& start, const glm::vec3& stop, 
		const std::vector<glm::vec3>& plane);

	/** Intersects two lines. Returns true, if intersection occured, false otherwise. */
    bool lineLineIntersection(
        const glm::vec3& p1, const glm::vec3& p, 
		const glm::vec3& pBorder, const glm::vec3& vBorder, 
		glm::vec3& ip);

	/** Callback function used to calculate the ghost plane when altering the plane normal */
    bool ghostPlane(core::param::ParamSlot& slot);

    /** Callback function to update the version after changing data */
    bool applyChanges(core::param::ParamSlot& slot);

	/** Callback function to set visibility of paramslots of corresponding plane mode */
    bool planeModeChanged(core::param::ParamSlot& slot);

    /**
     * Indirect Callback function if 'apply' is pressed.
	 * May be called if corresponding parameter slots are dirty.
     * Sets values of paramslots to all cooresponding param member variables.
	 * The corresponding parameter slots are:
	 * psStreamlineFieldName_
	 * psNumStreamlineSeeds_
	 * psStreamlineStepSize_
	 * psNumStreamlineSteps_
	 *
	 * Results in re-calculating the seeds and streamlines.
     */
    bool setStreamlineAndResampleSeedsUpdate();

	/** 
	* Indirect Callback function if 'apply' is pressed.
	* May be called if parameter slots planeColor and/or planeAlpha are dirty.
    * Sets values of paramslots to all cooresponding param member variables.
	*
	* Results in altering the mesh of the plane and sets new color/alpha values.
	*/
    bool setPlaneAndAppearanceUpdate();

	/** Callback function if seed re-sampling is set. */
    bool setResampleSeeds(core::param::ParamSlot& slot);

	/** Callback function to toggle the ghost plane. */
    bool toggleGhostPlane(core::param::ParamSlot& slot);

	/**
     * Indirect Callback function if 'apply' is pressed.
     * May be called if corresponding parameter slots are dirty.
     * Sets values of paramslots to all cooresponding param member variables.
     * The corresponding parameter slots are:
     * psPlaneOrigin_
     * psPlaneConnectionPoint1_
     * psPlaneConnectionPoint2_
     * psSeedPlaneNormal_
	 * psSeedPlanePoint_
     *
     * Results in re-calculating the plane, seeds, and streamlines.
     */
    bool setPlaneUpdate();

    /** Calculates intersectin points of sampling plane and bounding box */
    std::vector<glm::vec3> calcPlaneBboxIntersectionPoints(const visPlanef& samplePlane, const vtkm::Bounds& bounds);

    /**
     * Checks whether a point is outside the given boundaries.
     */
    bool isOutsideBbox(const glm::vec3& p, const vtkm::Bounds& bounds);

    /** Orders convex polygon vertices for a triangle fan */
    void orderPolygonVertices(std::vector<glm::vec3>& vertices);

    /** Decomposes a convex polygon into triangles */
    std::vector<Triangle> decomposePolygon(const std::vector<glm::vec3>& polygon);

    /** Checks whether a point is inside a given triangle */
    bool isInsideTriangle(const glm::vec3& p, const Triangle& tri);

    /** Creates and adds MeshDataAccessCollection to the mesh datacall */
    bool createAndAddMeshDataToCall(std::vector<glm::vec3>& data, std::vector<glm::vec4>& color,
        std::vector<unsigned int>& idcs, int numPoints, int numIndices,
        mesh::MeshDataAccessCollection::PrimitiveType pt = mesh::MeshDataAccessCollection::PrimitiveType::TRIANGLES);

    /** Adds the MeshDataAccessCollection to the mesh datacall */
    bool addMeshDataToCall(const std::vector<mesh::MeshDataAccessCollection::VertexAttribute>& va,
        const mesh::MeshDataAccessCollection::IndexData& id, mesh::MeshDataAccessCollection::PrimitiveType pt);

	/** Checks if a float is equal or really close to 0 */
    inline bool isZerof(float x) { return fabs(x) < epsilon5_; }
    inline bool isNullVector(const visVec3f& v) {
		return isZerof(v.GetX()) && isZerof(v.GetY()) && isZerof(v.GetZ());
    }
    inline bool isNullVector(const glm::vec3& v) { 
		return isZerof(v.x) && isZerof(v.y) && isZerof(v.z);
	}
	inline bool areParallel(const glm::vec3& v1, const glm::vec3& v2) { 
		// simple check if v1 is a multiple of v2 (or vice versa) is very imprecise
		// and causes false negatives
		// e. g. a = v1.x / v2.x = v1.y / v2.y = v1.z / v2.z
		return isNullVector(glm::cross(v1, v2));
	}
	inline bool isBetween(const glm::vec3& ip, const glm::vec3& p1, const glm::vec3& p2) {
        glm::vec3 vip = ip - p1;
        glm::vec3 vp2 = p2 - p1;
		float dot = glm::dot(vip, vp2);
        float lip = glm::length(vip);
        float lp2 = glm::length(vp2);

		return (dot >= 0.f) && (lip <= lp2);
	}

    /** Gets converted vtk streamline data as megamol mesh */
    core::CalleeSlot meshCalleeSlot_;

    /** Callerslot from which the vtk data is coming from */
    core::CallerSlot vtkCallerSlot_;

    /** Paramslot to specify the field name of streamline vector field */
    core::param::ParamSlot psStreamlineFieldName_;

    /** Paramslot to specify the seeds for the streamline */
    core::param::ParamSlot psNumStreamlineSeeds_;

    /** Paramslot to specify the step size of the streamline */
    core::param::ParamSlot psStreamlineStepSize_;

    /** Paramslot to specify the number of steps of the streamline */
    core::param::ParamSlot psNumStreamlineSteps_;

    /** Paramslot to specify the seeds for the streamline */
    core::param::ParamSlot psLowerStreamlineSeedBound_;

    /** Paramslot to specify the seeds for the streamline */
    core::param::ParamSlot psUpperStreamlineSeedBound_;

    /** Paramslot for plane mode */
    core::param::ParamSlot psSeedPlaneMode_;

	/** Paramslot for lower left corner of seed plane */
    core::param::ParamSlot psPlaneOrigin_;

    /** Paramslot for lower left corner of seed plane */
    core::param::ParamSlot psPlaneConnectionPoint1_;

    /** Paramslot for lower left corner of seed plane */
    core::param::ParamSlot psPlaneConnectionPoint2_;

    /** Paramslot for normal of seed plane */
    core::param::ParamSlot psSeedPlaneNormal_;

    /** Paramslot for point on seed plane */
    core::param::ParamSlot psSeedPlanePoint_;

	/** Paramslot for border line to bound seedplane */
    core::param::ParamSlot psSeedPlaneS_;

	/** Paramslot for border line to bound seedplane */
    core::param::ParamSlot psSeedPlaneT_;

	/** Paramslot for border line to bound seedplane */
	core::param::ParamSlot psSeedPlaneP_;

	/** Paramslot for border line to bound seedplane */
	core::param::ParamSlot psSeedPlaneQ_;

	/** Paramslot for resetting borderLines */
	//core::param::ParamSlot psResetBorderLines_;

	/** Paramslot to rotate ghostplane around its normal */
	core::param::ParamSlot psRotateGhostPlane_;

	/** Paramslot to rotate seedplane around its normal */
	core::param::ParamSlot psRotateSeedPlane_;

    /** Paramslot for seed plane color */
    core::param::ParamSlot psSeedPlaneColor_;

    /** Paramslot to apply changes to streamline configurations */
    core::param::ParamSlot psApplyChanges_;

	/** Paramslot to activate re-sampling of seeds */
    core::param::ParamSlot psResampleSeeds_;

	/** Paramslot to toggle the ghost plane */
    core::param::ParamSlot psToggleGhostPlane_;

	/** Update flags used to separate different calculations */
    bool streamlineUpdate_;
    bool planeUpdate_;
    bool planeAppearanceUpdate_;

	/** Hack to have the dirtyflag from the psSeedPlaneNormal after its callback function */
	bool planeNormalDirtyFlag_;

    /** Used for data version control, same as 'hash_data' */
    uint32_t newVersion_;

    /** Used for mesh data call */
    std::shared_ptr<mesh::MeshDataAccessCollection> meshDataAccess_;
    core::Spatial3DMetaData metaData_;

	/** Vtkm data structures */
    vtkm::cont::DataSet streamlineOutput_;
    vtkm::Bounds dataSetBounds_;
    float maxBoundLength_;

    /** Pointers to streamline data */
    std::vector<std::vector<glm::vec3>> streamlineData_;
    std::vector<std::vector<glm::vec4>> streamlineColor_;
    std::vector<std::vector<unsigned int>> streamlineIndices_;

    /** Data storage for streamline parameters */
    vtkm::Id numSeeds_;
    vtkm::Id numSteps_;
    vtkm::FloatDefault stepSize_;
    vislib::TString activeField_;
    glm::vec3 planeConnectionPoint1_;
    glm::vec3 planeConnectionPoint2_;
    glm::vec3 planeOrigin_;
    glm::vec3 seedPlaneNormal_;
	glm::vec3 seedPlaneZFightingOffset_;
    glm::vec3 seedPlanePoint_;
    glm::vec3 seedPlaneColor_;
    float seedPlaneAlpha_;

    std::vector<glm::vec3> liveSeedPlane_;
    std::vector<glm::vec3> liveCopy_;
    std::vector<glm::vec3> originalSeedPlane_;
    std::vector<glm::vec3> stpqSeedPlane_;
    std::vector<glm::vec4> seedPlaneColorVec_;
    std::vector<unsigned int> seedPlaneIndices_;
    std::vector<Triangle> seedPlaneTriangles_;
    std::vector<vtkm::Vec<vtkm::FloatDefault, 3>> seeds_;

    /** used for plane mode */
    int planeMode_;

    /** some colors for testing */
    glm::vec3 red_ = glm::vec3(0.5f, 0.f, 0.f);
    glm::vec3 orange_ = glm::vec3(0.5f, 0.25f, 0.f);
    glm::vec3 yellow_ = glm::vec3(0.5f, 0.5f, 0.f);
    glm::vec3 green_ = glm::vec3(0.f, 0.5f, 0.f);
    glm::vec3 cyan_ = glm::vec3(0.f, 0.5f, 0.5f);
    glm::vec3 blue_ = glm::vec3(0.f, 0.f, 0.5f);
    glm::vec3 purple_ = glm::vec3(0.5f, 0.f, 0.5f);
    glm::vec3 magenta_ = glm::vec3(0.25f, 0.f, 0.5f);
    glm::vec3 white = glm::vec3(1.f);
    glm::vec3 grey3 = glm::vec3(0.75f);
    glm::vec3 grey2 = glm::vec3(0.5f);
    glm::vec3 grey1 = glm::vec3(0.25f);
    glm::vec3 black = glm::vec3(0.f);
    std::vector<glm::vec3> colorVec_ = { red_, orange_, yellow_, green_, cyan_, blue_, purple_, magenta_ };
    
	std::vector<glm::vec3> ghostCopy_;
	std::vector<glm::vec3> rotatedGhostCopy_;
	std::vector<glm::vec3> ghostPlane_ = {
        glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)};
	std::vector<glm::vec4> ghostColors_ = {
        glm::vec4(red_, 0.2f), glm::vec4(green_, 0.2f), glm::vec4(blue_, 0.2f), glm::vec4(grey2, 0.2f)};
    std::vector<unsigned int> ghostIdcs_ = {0, 1, 2, 3};

	std::vector<glm::vec3> borderLine_ = {
        glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f),
        glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)};
    std::vector<glm::vec4> borderColors_ = {
        glm::vec4(grey2, 0.8f), glm::vec4(grey2, 0.8f),
		glm::vec4(grey2, 0.8f), glm::vec4(grey2, 0.8f),
		glm::vec4(grey2, 0.8f), glm::vec4(grey2, 0.8f),
        glm::vec4(grey2, 0.8f), glm::vec4(grey2, 0.8f)};
    std::vector<unsigned int> borderIdcs_ = {0, 1, 2, 3, 4, 5, 6, 7};

	float epsilon4_ = 1e-4f;
	float epsilon5_ = 1e-5f;
};

} // end namespace mmvtkm
} // end namespace megamol