const ifj = @import("ifj24.zig");
pub fn main() void {
    var x : i32 = 10;

    while (x > 0) | id_bez_null | {
        ifj.write("while\n");
        x = x - 1;
    }
}