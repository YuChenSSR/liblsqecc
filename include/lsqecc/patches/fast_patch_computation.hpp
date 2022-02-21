#ifndef LSQECC_FAST_PATCH_COMPUTATION_HPP
#define LSQECC_FAST_PATCH_COMPUTATION_HPP


#include <lsqecc/logical_lattice_ops/logical_lattice_ops.hpp>
#include <lsqecc/patches/slice.hpp>
#include <lsqecc/patches/patches.hpp>
#include <lsqecc/layout/layout.hpp>


namespace lsqecc {


class PatchComputation
{
public:

    PatchComputation (const LogicalLatticeComputation& logical_computation, std::unique_ptr<Layout>&& layout);

private:

    Slice& new_slice();
    Slice& last_slice();

    std::unique_ptr<Layout> layout_ = nullptr;
    std::vector<Slice> slices_;


public:
    const std::vector<Slice>& get_slices()const {return slices_;}
    const Layout& get_layout() const {return *layout_;}

};


}

#endif //LSQECC_FAST_PATCH_COMPUTATION_HPP
