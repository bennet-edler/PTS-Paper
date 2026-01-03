{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    gcc            # oder clang
    cmake
    git
    gtest # google test
    #gnumake
    #gdb            # Debugger
    #pkg-config
  ];

  # libs
  buildInputs = with pkgs; [
    #boost
  ];

  shellHook = ''
    echo "Development Environment active"
    echo "Compiler: $(gcc --version | head -n1)"
  '';
}
