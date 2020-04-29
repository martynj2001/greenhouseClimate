class PumpController < ApplicationController

  @@pumpStatus = '|0|'
  
  def pump_status
    render plain: @@pumpStatus
  end

  def set_pump_status
    @@pumpStatus = params[:status]
    if @@pumpStatus == "|1|"
        flash[:notice] = "You have turned the Garden watering system ON" 
    elsif @@pumpStatus == "|2|"
      flash[:notice] = "You have turned the Green House (only) watering system ON"
    else
        flash[:alert] = "The Watering Complete" 
    end
    redirect_to measures_path
  end

  helper_method :pumpStatus

  def pumpStatus
    @@pumpStatus
  end

end
