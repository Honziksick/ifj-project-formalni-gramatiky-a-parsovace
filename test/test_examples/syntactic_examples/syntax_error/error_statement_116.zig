// Syntax error: Missing right null condition bracket

const ifj = @import("ifj24.zig");

pub fn main() void {
    var x: i32 = 32;
    const y : i32 = 89;

    if(y < x) | id_bez_null
    {
        processItem(1458242);
    }
    else
    {
        _ = handleOddItem(53878);
    }
}