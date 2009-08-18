require 'hotkey_processor'

def slowly_do_stuff
  5.times { |i| sleep(0.1); print "#{i} " }
end

module HotkeyProcessor
  def self.process_keypress(code, down)
    print '. '
    Thread.new {slowly_do_stuff} if code == 65 and down # A pressed
    true
  end
end
puts 'Repeatedly Press A or other keys!'
HotkeyProcessor.start

