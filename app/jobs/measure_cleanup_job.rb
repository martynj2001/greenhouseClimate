class MeasureCleanupJob < ApplicationJob
  queue_as :default

  def perform(args)
    # Do something later
    # Rails.logger.debug "#{self.class.name}: I'm performing my job with arguments: #{args.inspect}"
    args.destroy
  end
end
