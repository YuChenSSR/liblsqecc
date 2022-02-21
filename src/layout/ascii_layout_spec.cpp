//
// Created by george on 2022-02-15.
//

#include <lsqecc/layout/ascii_layout_spec.hpp>
#include <absl/strings/str_split.h>
#include <cppitertools/itertools.hpp>

#include <iterator>
#include <algorithm>
#include <numeric>
#include <deque>

static_assert(std::is_same_v<absl::string_view,std::string_view>);

namespace lsqecc{
std::vector<std::vector<AsciiLayoutSpec::CellType>> AsciiLayoutSpec::parse_grid(const std::string_view input)
{
    std::vector<std::vector<AsciiLayoutSpec::CellType>> rows;
    for(const std::string_view& row: absl::StrSplit(input,'\n'))
    {
        rows.emplace_back();
        for(char c: row)
        {
            rows.back().push_back(cell_type_from_char(c));
        }
    }
    return rows;
}

std::vector<Cell> AsciiLayoutSpec::connected_component(Cell start) const
{
    AsciiLayoutSpec::CellType target_type = grid_spec_[start.row][start.col];
    std::vector<Cell> connected_cells;
    std::deque<Cell> fronteer;
    fronteer.push_back(start);

    while(fronteer.size()>0)
    {
        Cell curr = fronteer.front(); fronteer.pop_front();
        connected_cells.push_back(curr);
        auto neighbours = curr.get_neigbours_within_bounding_box_inclusive({0,0}, furthest_cell());
        for (const auto &item : neighbours)
        {
            if(grid_spec_[item.row][item.col] == target_type
            && std::find(fronteer.begin(), fronteer.end(), item) == fronteer.end()
            && std::find(connected_cells.begin(), connected_cells.end(), item) == connected_cells.end())
            {
                fronteer.push_back(item);
            }
        }
    }

    return connected_cells;
}

Cell AsciiLayoutSpec::furthest_cell() const
{
    if(grid_spec_.size()==0) throw std::logic_error("Grid spec with no rows");

    size_t n_cols = grid_spec_.front().size();
    for (const auto &row : grid_spec_)
        if(n_cols != row.size())
            throw std::logic_error("All rows must have the same number of cols");

    if(n_cols==0) throw std::logic_error("Grid spec with no cols");
    return Cell::from_ints(grid_spec_.size()-1, n_cols-1);
}


MultipleCellsOccupiedByPatch make_distillation_region_starting_from(const Cell& cell, const AsciiLayoutSpec& spec)
{
    MultipleCellsOccupiedByPatch region;
    auto single_cells = iter::imap(LayoutHelpers::make_distillation_region_cell, spec.connected_component(cell));
    std::move(single_cells.begin(), single_cells.end(), std::back_inserter(region.sub_cells));
    return region;
}

std::optional<Cell> AsciiLayoutSpec::find_a_cell_of_type(AsciiLayoutSpec::CellType target) const
{
    for(const auto&[row, row_data]: iter::enumerate(grid_spec_))
        for(const auto&[col, cell]: iter::enumerate(row_data))
            if(cell == target)
                return Cell{static_cast<Cell::CoordinateType>(row), static_cast<Cell::CoordinateType>(col)};

    return std::nullopt;
}

std::ostream& AsciiLayoutSpec::operator<<(std::ostream& os) {
    for (const auto &row : grid_spec_)
    {
        for (const auto &item : row)
            os << item;
        os << std::endl;
    }
    return os;
}

void LayoutFromSpec::init_cache(const AsciiLayoutSpec& spec)
{
    for(const AsciiLayoutSpec::CellType distillation_region_char : AsciiLayoutSpec::k_distillation_region_types)
    {
        auto a_cell_for_this_region = spec.find_a_cell_of_type(distillation_region_char);
        if (a_cell_for_this_region)
        {
            cached_distillation_regions_.push_back(
                    make_distillation_region_starting_from(*a_cell_for_this_region, spec));
            cached_distillation_times_.push_back(5);
        }
    }

    for(const auto&[row, row_data]: iter::enumerate(spec.get_grid_spec()))
        for(const auto&[col, cell]: iter::enumerate(row_data))
            if(cell == AsciiLayoutSpec::LogicalComputationQubit_StandardBorderOrientation)
                cached_core_patches_.push_back(LayoutHelpers::basic_square_patch(Cell::from_ints(row,col)));

    cached_min_furthest_cell_ = spec.furthest_cell();
}


}
