const std = @import("std");
const zcc = @import("compile_commands");

const Compile = std.Build.Step.Compile;

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe_mod = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    const exe = b.addExecutable(.{
        .name = "piss",
        .root_module = exe_mod,
    });
    b.installArtifact(exe);

    exe.addCSourceFiles(.{
        .files = src_files,
        .flags = cflags,
        .root = b.path("src"),
    });
    exe.addIncludePath(b.path("include"));

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    var targets = std.ArrayList(*Compile).init(b.allocator);
    try targets.append(exe);
    zcc.createStep(b, "cdb", try targets.toOwnedSlice());
}

const src_files = &.{
    "main.c",
    "gpio.c",
    "pid.c",
    "i2c.c",
    "pca9685.c",
    "motor.c",
};

const cflags = &.{ "-Wall", "-Wextra", "-Werror" };
