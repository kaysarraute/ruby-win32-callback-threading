require 'hotkey_processor'

def slowly_do_stuff
  5.times { |i| sleep(0.1); print "#{i} " }
end

module HotkeyProcessor
  def self.process_keypress(code, down)
    print '. '
    slowly_do_stuff              if code == 65 and down # A pressed
    Thread.new {slowly_do_stuff} if code == 66 and down # B pressed
    true
  end
end
puts 'Press A, B and other keys.'
HotkeyProcessor.start

