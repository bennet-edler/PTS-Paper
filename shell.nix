{ pkgs ? import <nixpkgs> {} }:


let
  Python = pkgs.python3.withPackages (ps: with ps; [
    matplotlib
    pandas
    # numpy
  ]);
in
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    gcc         
    cmake
    git
    gtest # google test
    #gnumake
    gdb            # Debugger
    #pkg-config
    Python
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
