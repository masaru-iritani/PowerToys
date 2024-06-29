#include <common/Shortcut.h>
#include <vector>

class RemapBuffer
{
public:
    RemapBuffer(std::initializer_list<RemapBufferRow> list) : rows(list) {}

    constexpr void Add(RemapBufferRow row) { rows.push_back(row); }
    constexpr void Clear() noexcept { rows.clear(); }
    constexpr bool IsEmpty() const noexcept { return rows.empty(); }
    constexpr void RemoveAt(size_t index) noexcept { rows.erase(rows.begin() + index); }
    constexpr size_t Size() const noexcept { return rows.size(); }

    constexpr RemapBufferRow operator[](size_t index) const { return rows.at(index); }
    constexpr auto begin() const noexcept { return rows.begin(); }
    constexpr auto end() const noexcept { return rows.end(); }

private:
    std::vector<RemapBufferRow> rows;
};
