-- Spaceship Scene:
-- Spaceship scene
-- with the provided skeleton code.  It doesn't rely on hierarchical
-- transformations.

-- Create the top level root node named 'root'.]
root_node = gr.node('real_root')
root_node = gr.node('root')
-- root_node:rotate('y', 90)

-- colors
red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
cyan = gr.material({0.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

-- BODY AND COCKPIT
cockpit = gr.mesh('cube', 'cockpit')
root_node:add_child(cockpit)
cockpit:set_material(black)
cockpit:scale(0.3, 0.15, 0.5)

behind_cockpit = gr.mesh('cube', 'behind_cockpit')
cockpit:add_child(behind_cockpit)
behind_cockpit:set_material(white)
behind_cockpit:translate(0, 0, -1.0)

body_back = gr.mesh('cube', 'body_back')
behind_cockpit:add_child(body_back)
body_back:set_material(white)
body_back:scale(2.0, 2.0, 1.5)
body_back:translate(0, -1.5, 0.25)

body_cube1 = gr.mesh('cube', 'body_cube1')
root_node:add_child(body_cube1)
body_cube1:set_material(white)
body_cube1:scale(0.15, 0.15, 0.15)
body_cube1:translate(0.225, -0.15, 0.075)

body_cube2 = gr.mesh('cube', 'body_cube2')
root_node:add_child(body_cube2)
body_cube2:set_material(white)
body_cube2:scale(0.15, 0.15, 0.15)
body_cube2:translate(-0.225, -0.15, 0.075)

body_front = gr.mesh('cube', 'body_front')
cockpit:add_child(body_front)
body_front:set_material(red)
body_front:scale(1.0, 2.0, 1.0)
body_front:translate(0, -1.5, 0.5)

front_tip = gr.mesh('cube', 'front_tip')
body_front:add_child(front_tip)
front_tip:set_material(white)
front_tip:scale(0.8, 0.8, 0.25)
front_tip:translate(0, 0, 0.5)

-- LEFT WING
lwing1 = gr.mesh('cube', 'lwing1')
body_back:add_child(lwing1)
lwing1:set_material(red)
lwing1:scale(0.25, 0.5, 0.85)
lwing1:translate(-0.6, 0, 0)

lwing2 = gr.mesh('cube', 'lwing2')
lwing1:add_child(lwing2)
lwing2:set_material(red)
lwing2:scale(1, 0.8, 0.75)
lwing2:translate(-1, 0, -0.125)

lwing3 = gr.mesh('cube', 'lwing3')
lwing2:add_child(lwing3)
lwing3:set_material(red)
lwing3:scale(1, 0.8, 1)
lwing3:translate(-1, 0, 0)

lwing4 = gr.mesh('cube', 'lwing4')
lwing3:add_child(lwing4)
lwing4:set_material(red)
lwing4:scale(1, 0.8, 0.75)
lwing4:translate(-1, 0, -0.125)

-- RIGHT WING
rwing1 = gr.mesh('cube', 'rwing1')
body_back:add_child(rwing1)
rwing1:set_material(red)
rwing1:scale(0.25, 0.5, 0.85)
rwing1:translate(0.6, 0, 0)

rwing2 = gr.mesh('cube', 'rwing2')
rwing1:add_child(rwing2)
rwing2:set_material(red)
rwing2:scale(1, 0.8, 0.75)
rwing2:translate(1, 0, -0.125)

rwing3 = gr.mesh('cube', 'rwing3')
rwing2:add_child(rwing3)
rwing3:set_material(red)
rwing3:scale(1, 0.8, 1)
rwing3:translate(1, 0, 0)

rwing4 = gr.mesh('cube', 'rwing4')
rwing3:add_child(rwing4)
rwing4:set_material(red)
rwing4:scale(1, 0.8, 0.75)
rwing4:translate(1, 0, -0.125)

-- BACK ENGINE
back_engine = gr.mesh('cube', 'back_engine')
root_node:add_child(back_engine)
back_engine:set_material(cyan)
back_engine:scale(0.25, 0.15, 0.25)
back_engine:translate(0, -0.2, -0.75)

return root_node
