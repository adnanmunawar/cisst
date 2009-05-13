/********************************
 PLACEHOLDER STRINGS TO LOOK FOR:

 TODO       todo
********************************/

#include <cisstVector.h>
#include <iostream>

// TODO: fix the CMakeLists.txt file

typedef unsigned int size_type;

template <class _elementType>
class vctDynamicMatrixTypemapsTest
{

protected:

    vctDynamicMatrix<_elementType> copy;

public:

    vctDynamicMatrixTypemapsTest()
    {}

    void in_argout_vctDynamicMatrix_ref(vctDynamicMatrix<_elementType> &param, size_type sizeFactor) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
        param += 1;

        if (sizeFactor != 0) {
            size_type rowsOld = param.rows();
            size_type colsOld = param.cols();
            size_type rowsNew = rowsOld * sizeFactor;
            size_type colsNew = colsOld * sizeFactor;
            param.resize(rowsNew, colsNew);

            // TODO: is there a better way to do this?
            for (size_type r = 0; r < rowsNew; r++) {
                for (size_type c = 0; c < colsNew; c++) {
                    param.at(r, c) = param.at(r % rowsOld, c % colsOld);
                }
            }
        }
    }

    void in_vctDynamicMatrixRef(vctDynamicMatrixRef<_elementType> param, size_type dummy) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
        param += 1;
    }

    void in_vctDynamicConstMatrixRef(vctDynamicConstMatrixRef<_elementType> param, size_type dummy) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicConstMatrixRef_ref(const vctDynamicConstMatrixRef<_elementType> &param, size_type dummy) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicMatrixRef_ref(const vctDynamicMatrixRef<_elementType> &param, size_type dummy) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_vctDynamicMatrix(vctDynamicMatrix<_elementType> param, size_type dummy) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    void in_argout_const_vctDynamicMatrix_ref(const vctDynamicMatrix<_elementType> &param, size_type dummy) {
        copy.SetSize(param.sizes());
        copy.Assign(param);
    }

    vctDynamicMatrix<_elementType> out_vctDynamicMatrix(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);     // TODO: this is actually not random!
        return copy;
    }

    vctDynamicMatrix<_elementType> &out_vctDynamicMatrix_ref(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);     // TODO: this is actually not random!
        return copy;
    }

    const vctDynamicMatrix<_elementType> &out_const_vctDynamicMatrix_ref(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);     // TODO: this is actually not random!
        return copy;
    }

    vctDynamicMatrixRef<_elementType> out_vctDynamicMatrixRef(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);     // TODO: this is actually not random!
        return copy;
    }

    vctDynamicConstMatrixRef<_elementType> out_vctDynamicConstMatrixRef(size_type rows, size_type cols) {
        copy.SetSize(rows, cols);
        vctRandom(copy, 0, 10);     // TODO: this is actually not random!
        return copy;
    }

    inline _elementType GetItem(size_type rowIndex, size_type colIndex) const
    throw(std::out_of_range) {
        return copy.at(rowIndex, colIndex);
    }

    inline void SetItem(size_type rowIndex, size_type colIndex, _elementType value)
    throw(std::out_of_range) {
        copy.at(rowIndex, colIndex) = value;
    }

    inline size_type rows() const {
        return copy.rows();
    }

    inline size_type cols() const {
        return copy.cols();
    }
};