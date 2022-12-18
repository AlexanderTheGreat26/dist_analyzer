from sys import argv
import vmd
from vmd import molecule

input_file_name = argv[1]
output_file_name = argv[2]

mol = vmd.molecule.load("pdb", input_file_name)
molecule.write(mol, "xyz", output_file_name)

